#ifndef SERVER_H_
#define SERVER_H_

#include <memory>

class DbConnector;

class Server {
  using TcpCallback = void (*)(struct ev_loop*, struct ev_io*, int);
  using UdpCallback = void (*)(std::unique_ptr<char[]>&&, DbConnector&);

 public:
  Server();
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

