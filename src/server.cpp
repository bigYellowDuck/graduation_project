#include "server.h"
#include "event_loop.h"
#include "acceptor.h"
#include "udpkeeper.h"
#include "inet_address.h"
#include "logging.h"
#include "configer.h"

struct Server::Impl {
  Impl(const std::string& conf_path);

  Configer configer_;
  EventLoop baseloop_;
  MultiEventLoop multiloop_;
  InetAddress tcpaddr_;
  InetAddress udpaddr_;
  Acceptor acceptor_;
  Udpkeeper udpkeeper_;
};

Server::Impl::Impl(const std::string& conf_path)
  : configer_(conf_path),
    baseloop_(), 
    multiloop_(&baseloop_, configer_.ThreadNum()),
    tcpaddr_(configer_.TcpPort()),
    udpaddr_(configer_.UdpPort()),
    acceptor_(tcpaddr_, &baseloop_), 
    udpkeeper_(udpaddr_, &baseloop_) {
  setlogfile(configer_.LogfileName());
  setloglevel(configer_.Loglevel());
}

Server::Server(const std::string& conf_path)
  : pimpl_(std::move(std::unique_ptr<Impl>(new Impl(conf_path)))) {
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
