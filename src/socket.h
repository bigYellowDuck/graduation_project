#ifndef SOCKET_H_
#define SOCKET_H_

#include <utility>
#include <memory>

class InetAddress;

class Socket {
 public:
  explicit Socket(int sockfd);
  virtual ~Socket();
  
  Socket(const Socket& rhs) = delete;
  Socket& operator=(const Socket& rhs) = delete;

  Socket(Socket&& rhs);
  Socket& operator=(Socket&& rhs);

  void Swap(Socket &rhs);

  int Fd() const { return sockfd_; }

  void BindOrDie(const InetAddress& addr);
  
  // return 0 on success
  int Connect(const InetAddress& addr);

  // pure io function
  virtual int Read(void* buf, int len) const = 0;
  virtual int Write(const void* buf, int len) const = 0;
 protected:
  int sockfd_;
};


class TcpSocket : public Socket {
 public:
  explicit TcpSocket(int sockfd);
  ~TcpSocket() = default;
  
  TcpSocket(TcpSocket&& rhs);
  TcpSocket& operator=(TcpSocket&& rhs);

  void ListenOrDie();

  void ShutdownWrite();
  
  void SetReuseAddr(bool on);
  void SetTcpNoDelay(bool on);
  
  InetAddress GetLocalAddr() const;
  InetAddress GetPeerAddr() const;

  int Read(void* buf, int len) const;
  int Write(const void* buf, int len) const;

  //factory methods
  static TcpSocket CreateTcpSocket();
  static std::unique_ptr<TcpSocket> CreateTcpSocketPtr();
};


class UdpSocket : public Socket {
 public:
  explicit UdpSocket(int sockfd);
  ~UdpSocket() = default;

  UdpSocket(UdpSocket&& rhs);
  UdpSocket& operator=(UdpSocket&& rhs);

  int Read(void* buf, int len) const;
  int Write(const void* buf, int len) const;

  static UdpSocket CreateUdpSocket();
  static std::unique_ptr<UdpSocket> CreateUdpSocketPtr();
};

#endif  // SOCKET_H_

