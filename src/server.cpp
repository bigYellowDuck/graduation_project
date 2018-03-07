#include "server.h"
#include "event_loop.h"
#include "acceptor.h"
#include "udpkeeper.h"
#include "inet_address.h"
#include "logging.h"
#include "configer.h"

struct Server::Impl {
  Impl();

  EventLoop baseloop_;
  MultiEventLoop multiloop_;
  InetAddress tcpaddr_;
  InetAddress udpaddr_;
  Acceptor acceptor_;
  Udpkeeper udpkeeper_;
};

Server::Impl::Impl()
  : baseloop_(), 
    multiloop_(&baseloop_, Configer::Instance().ThreadNum()),
    tcpaddr_(Configer::Instance().TcpPort()),
    udpaddr_(Configer::Instance().UdpPort()),
    acceptor_(tcpaddr_, &baseloop_), 
    udpkeeper_(udpaddr_, &baseloop_) {
}

Server::Server()
  : pimpl_(std::move(std::unique_ptr<Impl>(new Impl))) {
  info("Server construct success.");
}

Server::~Server() = default;

void Server::SetTcpCallback(TcpCallback callback) {
  (pimpl_->acceptor_).setConnectCallback(callback);
}

void Server::SetUdpCallback(UdpCallback callback) {
  (pimpl_->udpkeeper_).setReadCallBack(callback);
}

void Server::Run() {
  info("Server is running.");
  (pimpl_->multiloop_).Start();
}
