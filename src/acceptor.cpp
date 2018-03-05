#include "acceptor.h"
#include "event_loop.h"
#include "logging.h"

Acceptor::Callback Acceptor::callback_ = NULL;

Acceptor::Acceptor(const InetAddress& listenaddr, EventLoop* loop) 
  : listen_socket_(TcpSocket::CreateTcpSocket()),
    loop_(loop) {
  listen_socket_.SetReuseAddr(true);
  listen_socket_.BindOrDie(listenaddr);
  listen_socket_.ListenOrDie();
  loop_->AddIoEvent(listen_socket_.Fd(), EV_READ, HandleRead);
  info("Accetor begin to listen: %s", listenaddr.ToIpPort().data());
}

void Acceptor::HandleRead(struct ev_loop* loop, struct ev_io* watcher, int revents) {
  struct sockaddr_in peeraddr;
  socklen_t addrlen;
  int sockfd = ::accept4(watcher->fd, (struct sockaddr*)&peeraddr, &addrlen, SOCK_CLOEXEC);
  InetAddress peeraddress(peeraddr);
  if (sockfd > 0) {
    info("Acceptor build tcp connection with %s", peeraddress.ToIpPort().data());
    std::unique_ptr<Socket> socket = std::unique_ptr<TcpSocket>(new TcpSocket(sockfd));
    EventLoop* eloop = static_cast<EventLoop*>(ev_userdata(loop));
    EventLoop* l = eloop->GetMultiEventLoop()->GetNextLoop();
    l->AddSocket(std::move(socket), EV_READ, callback_);
    l->WakeUp();
  } else {
    error("Acceptor accept socket failed");
  }
  (void)revents;
}

void Acceptor::Echo(struct ev_loop* loop, struct ev_io* watcher, int revents) {
  trace("Acceptor - Echo ");
  const Socket *socket = static_cast<const Socket*>(watcher->data);
  char buf[1024] = {0};
  int nread = socket->Read(buf, sizeof(buf));
  socket->Write(buf, nread);
  EventLoop* eloop = static_cast<EventLoop*>(ev_userdata(loop)); 
  eloop->RemoveSocket(watcher);
  (void)revents;
}

void Acceptor::setConnectCallback(Callback callback) {
  callback_ = callback;
}
