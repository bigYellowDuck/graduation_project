#include "udpkeeper.h"
#include "event_loop.h"
#include "logging.h"

Udpkeeper::Udpkeeper(const InetAddress& udpaddr, EventLoop* loop)
  : socket_(UdpSocket::CreateUdpSocket()),
    loop_(loop) {
  socket_.BindOrDie(udpaddr);
  struct ev_io* watcher = loop_->AddIoEvent(socket_.Fd(), EV_READ, HandleRead);
  watcher->data = &socket_;
  info("Udpkeep begin to receive datagram form: %s", udpaddr.ToIpPort().data());
}

void Udpkeeper::HandleRead(struct ev_loop* loop, struct ev_io* watcher, int revents) {
  const UdpSocket* socket = static_cast<const UdpSocket*>(watcher->data);
  std::unique_ptr<char[]> buf(new char[1024]{0});
  socket->Read(buf.get(), 1024);
  info("unique_ptr data : %s", buf.get());
  EventLoop* eloop = static_cast<EventLoop*>(ev_userdata(loop));
  EventLoop* worker_loop = eloop->GetMultiEventLoop()->GetNextLoop();
  BlockingQueue<std::unique_ptr<char[]>>& queue = worker_loop->GetBlockingQueueRef();
  queue.push(std::move(buf));
  worker_loop->WakeUp();

  (void)revents;
}

void Udpkeeper::Discard(struct ev_loop* loop, struct ev_io* watcher, int revents) {
  (void)watcher;
  (void)loop;
  (void)revents;
}

void Udpkeeper::setReadCallBack(Callback callback) {
  EventLoop::SetHandleUdpCallbak(callback);
}
