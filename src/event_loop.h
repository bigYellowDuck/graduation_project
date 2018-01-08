#ifndef EVENT_LOOP_H_
#define EVENT_LOOP_H_

#include "threads.h"
#include "blocking_queue.h"

#include <ev.h>
#include <vector>
#include <map>

class Socket;
class MultiEventLoop;

class EventLoop {
  using IoEventCallback = void (*)(struct ev_loop*, struct ev_io*, int);
  using TimerEventCallback = void(*)(struct ev_loop*, struct ev_timer*, int);
  using UdpCallback = void(*)(std::unique_ptr<char[]>&&);
 public:
  explicit EventLoop();
  ~EventLoop();

  EventLoop(const EventLoop& rhs) = delete;
  EventLoop& operator=(const EventLoop& rhs) = delete;

  EventLoop(EventLoop&& rhs) = default;

  struct ev_io* AddIoEvent(const int fd, int events, IoEventCallback callbak);
  // bool UpdateIoEvent(int fd, int events, IoEventCallback callback);
  void RemoveIoEvent(struct ev_io* watcher);

  struct ev_timer* AddTimerEvent(ev_tstamp after, ev_tstamp repeat, TimerEventCallback callback);
  void RemoveTimerEvent(struct ev_timer* timer);

  struct ev_io* AddSocket(std::unique_ptr<Socket>&& socket, int events,  IoEventCallback callback);
  void RemoveSocket(struct ev_io* watcher);

  void WakeUp();

  void Loop() { ev_run(loop_, 0); }

  void SetMultiEventLoop(MultiEventLoop* mloop) { mloop_ = mloop; }
  MultiEventLoop* GetMultiEventLoop() const { return mloop_; }

  BlockingQueue<std::unique_ptr<char[]>>& GetBlockingQueueRef() {
    return udpdata_queue_;
  }

  static void SetHandleUdpCallbak(UdpCallback callback) { udp_callback_ = callback; }
 private:
  struct ev_loop* loop_;
  struct ev_io* wakeup_watcher_;
  std::map<int, std::unique_ptr<Socket>> sockfds_;
  MultiEventLoop* mloop_;
  BlockingQueue<std::unique_ptr<char[]>> udpdata_queue_;
  
  static UdpCallback udp_callback_;

  static void AwakeAndHandleEvents(struct ev_loop* loop, struct ev_io*, int revents);
};

class MultiEventLoop {
 public:
  MultiEventLoop(EventLoop* baseloop, size_t threads);
  ~MultiEventLoop();
  
  MultiEventLoop(const MultiEventLoop&) = delete;
  MultiEventLoop& operator=(const MultiEventLoop&) = delete;
  
  void Start();
  EventLoop* GetNextLoop();

 private:
  EventLoop* base_loop_;
  size_t threads_;
  ThreadPool thread_pool_;
  std::vector<std::unique_ptr<EventLoop>> loops_;
  bool runing_;
  size_t next_;
};

#endif  // EVENT_LOOP_H_

