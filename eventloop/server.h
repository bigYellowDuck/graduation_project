#ifndef SERVER_H_
#define SERVER_H_

#include "configer.h"

#include <memory>

class Server {
  using TcpCallback = void (*)(struct ev_loop*, struct ev_io*, int);
  using UdpCallback = void (*)(std::unique_ptr<char[]>&&);

 public:
  Server(const std::string& conf_path);
  ~Server();

  Server(const Server&) = delete;
  Server& operator=(const Server&) = delete;
  Server(Server&&) = delete;
  Server& operator=(Server&&) = delete;

  void SetTcpCallback(TcpCallback callback);
  void SetUdpCallback(UdpCallback callback);

  void Run();

 private:
  struct Impl;
  std::unique_ptr<Impl> pimpl_;
};

#endif // SERVER_H_

