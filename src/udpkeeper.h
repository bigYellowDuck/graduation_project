#ifndef UDPKEEPER_H_
#define UDPKEEPER_H_

#include "inet_address.h"
#include "socket.h"
#include "db_connector.h"

class EventLoop;

class Udpkeeper {
  using Callback = void (*)(std::unique_ptr<char[]>&&, DbConnector&);
 public:
  Udpkeeper(const InetAddress& udpaddr, EventLoop* loop);
  ~Udpkeeper() = default;
  
  Udpkeeper(const Udpkeeper&) = delete;
  Udpkeeper& operator=(const Udpkeeper) = delete;

  int Fd() const { return socket_.Fd(); }

  static void HandleRead(struct ev_loop* loop, struct ev_io* watcher, int revents);

  static void Discard(struct ev_loop* loop, struct ev_io* watcher, int revents);

  static void setReadCallBack(Callback callback);
 private:
  UdpSocket socket_;
  EventLoop* loop_;
};

#endif //  UDPKEEPER_H_

