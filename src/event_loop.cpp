#include "event_loop.h"
#include "util.h"
#include "socket.h"
#include "logging.h"

#include <string.h>
#include <unistd.h>

EventLoop::UdpCallback EventLoop::udp_callback_ = NULL;

EventLoop::EventLoop()
   : loop_(ev_loop_new(EVFLAG_AUTO)) {
  wakeup_watcher_ = AddIoEvent(util::CreateEventFd(), EV_READ, AwakeAndHandleEvents);
  ev_set_userdata(loop_, this);
}

EventLoop::~EventLoop() {
  RemoveIoEvent(wakeup_watcher_);
  ev_loop_destroy(loop_);
}

struct ev_io* EventLoop::AddIoEvent(const int fd, 
                                    int events, 
                                    IoEventCallback callback) {
  struct ev_io* watcher = (struct ev_io*)malloc(sizeof(struct ev_io));
  if (watcher != NULL) {
    memset(watcher, 0, sizeof(struct ev_io));
    ev_init(watcher, callback);
    ev_io_set(watcher, fd, events);
    ev_io_start(loop_, watcher);
  }
  info("EventLoop add io event, watcher %p, fd %d, event %d", watcher, fd, events);
  return watcher;
}

void EventLoop::RemoveIoEvent(struct ev_io* watcher) {
  if (watcher != NULL) {
    info("EventLoop remove io event, watcher %p, fd %d", watcher, watcher->fd);
    close(watcher->fd);
    ev_io_stop(loop_, watcher);
    free(watcher);
    watcher = NULL;
  }
}

struct ev_timer* EventLoop::AddTimerEvent(ev_tstamp after, 
                                          ev_tstamp repeat, 
                                          TimerEventCallback callback) {
  struct ev_timer* watcher = (struct ev_timer*)malloc(sizeof(struct ev_timer));
  if (watcher != NULL) {
    memset(watcher, 0, sizeof(struct ev_timer));
    ev_init(watcher, callback);
    ev_timer_set(watcher, after, repeat);
    ev_timer_start(loop_, watcher);
  }
  info("EventLoop add timer event, watcher %p, after %lf, repeat %lf",watcher, after, repeat);
  return watcher;
}

void EventLoop::RemoveTimerEvent(struct ev_timer* watcher) {
  if (watcher) {
    info("EventLoop remove timer event, watcher %p", watcher);
    ev_timer_stop(loop_, watcher);
    free(watcher);
    watcher = NULL;
  }
}

struct ev_io* EventLoop::AddSocket(std::unique_ptr<Socket>&& socket, 
                                   int events, 
                                   IoEventCallback callback) {
  struct ev_io* watcher = (struct ev_io*)malloc(sizeof(ev_io));
  int fd = -1;
  if (watcher != NULL) {
    memset(watcher, 0, sizeof(struct ev_io));
    ev_init(watcher, callback);
    ev_io_set(watcher, socket->Fd(), events);
    ev_io_start(loop_, watcher);
    
    fd = socket->Fd();
    sockfds_[fd].swap(socket);
    watcher->data = sockfds_[fd].get();
  }
  info("EventLoop add socket event, watcher %p, sockfd %d, events %d", watcher, fd, events); 
  return watcher;
}

void EventLoop::RemoveSocket(struct ev_io* watcher) {
  if (watcher) {
    info("EventLoop remove socket event, watcher %p, sockfd %d", watcher, watcher->fd);
    ev_io_stop(loop_, watcher);
    sockfds_.erase(watcher->fd);
    free(watcher);
    watcher = NULL;
  }
}

void EventLoop::WakeUp() {
  uint64_t n = 1;
  int r = ::write(wakeup_watcher_->fd, &n, sizeof(uint64_t));
}

void EventLoop::AwakeAndHandleEvents(struct ev_loop* loop, struct ev_io* watcher, int revents) {
  (void)loop; (void) revents;
  uint64_t n;
  int r = ::read(watcher->fd, &n, sizeof(uint64_t));
  EventLoop* eloop = static_cast<EventLoop*>(ev_userdata(loop));
  BlockingQueue<std::unique_ptr<char[]>>& queue = eloop->GetBlockingQueueRef(); 
  size_t cursize = queue.size();
  info("Currently EventLoop %p blocking queue has %u uniptr", eloop, static_cast<unsigned>(cursize));
  for (size_t i=0; i!=cursize; i++) {
    std::unique_ptr<char[]> data;
    queue.pop(&data);
    trace("udp datagram: %s", data.get());
    udp_callback_(std::move(data));
  }
}



MultiEventLoop::MultiEventLoop(EventLoop* baseloop, size_t threads) 
   : base_loop_(baseloop),
     threads_(threads-1), 
     thread_pool_(threads-1),
     runing_(false),
     next_(0) {
  base_loop_->SetMultiEventLoop(this);
}

MultiEventLoop::~MultiEventLoop() {
  if (runing_) {
    thread_pool_.Exit();
  }
}

void MultiEventLoop::Start() {
  if (runing_) {
    return;
  }

  loops_.resize(threads_);
  thread_pool_.Start();
  
  for (size_t i = 0; i < threads_; ++i) {
    thread_pool_.AddTask(
      [this, i] {
        loops_[i].reset(new EventLoop);
        loops_[i]->SetMultiEventLoop(this);
        loops_[i]->Loop();
      }    
    );
  }

  runing_ = true;
  info("MultiEventLoop has run %lu eventloops", threads_+1);

  base_loop_->Loop();
}

EventLoop* MultiEventLoop::GetNextLoop() {
  EventLoop* loop = base_loop_;
  if (!loops_.empty()) {
    loop = loops_[next_++].get();
    if (next_ >= loops_.size()) {
      next_ = 0;
    }
  }
  return loop;
}

