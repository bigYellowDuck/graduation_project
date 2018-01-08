#ifndef ACCEPTOR_H_
#define ACCEPTOR_H_

#include "inet_address.h"
#include "socket.h"

#include <ev.h>

class EventLoop;

class Acceptor {
  using Callback = void (*)(struct ev_loop* loop, struct ev_io* watcher, int revents);
 public:
  Acceptor(const InetAddress& listenaddr, EventLoop* loop);
  ~Acceptor() = default;
 
  Acceptor(const Acceptor&) = delete;
  Acceptor& operator=(const Acceptor&) = delete;

  int Fd() const { return listen_socket_.Fd(); }

  static void HandleRead(struct ev_loop* loop, struct ev_io* watcher, int revents);
  
  static void Echo(struct ev_loop* loop, struct ev_io* watcher, int revents);
  
  static void setConnectCallback(Callback callback);
 private:
  TcpSocket listen_socket_;
  EventLoop* loop_;
  static Callback callback_;
};

#endif  // ACCEPTOR_H_
