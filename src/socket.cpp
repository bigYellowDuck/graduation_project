#include "socket.h"
#include "inet_address.h"
#include "logging.h"

#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <netinet/tcp.h>

Socket::Socket(int sockfd)
  : sockfd_(sockfd) {
  assert(sockfd_ > 0);
  trace("Socket %d have created", sockfd_);
}

Socket::~Socket() {
  close(sockfd_);
  trace("Socket %d have closed", sockfd_);  
}

Socket::Socket(Socket&& rhs)
  : sockfd_(rhs.sockfd_) {
  rhs.sockfd_ = -1;
}

Socket& Socket::operator=(Socket&& rhs) {
  Swap(rhs);
  return *this;
}

void Socket::Swap(Socket &rhs) {
  std::swap(sockfd_, rhs.sockfd_);
}

void Socket::BindOrDie(const InetAddress& addr) {
  const struct sockaddr_in &saddr = addr.GetSockAddrInet();
  int ret = ::bind(sockfd_, (const struct sockaddr*)&saddr, sizeof(saddr));
  fatalif(ret<0, "Socket bind error, errno %d", errno);
  info("Socket %d has bind %s", sockfd_, addr.ToIpPort().data());
}

int Socket::Connect(const InetAddress& addr) {
  const struct sockaddr_in &saddr = addr.GetSockAddrInet();
  return ::connect(sockfd_, (const struct sockaddr*)&saddr, sizeof(saddr));
}

TcpSocket::TcpSocket(int sockfd) 
  : Socket(sockfd) {
}

TcpSocket::TcpSocket(TcpSocket&& rhs)
  : Socket(std::move(rhs)) {
}

TcpSocket& TcpSocket::operator=(TcpSocket&& rhs) {
  std::swap(sockfd_, rhs.sockfd_);
  return *this;
}

void TcpSocket::ListenOrDie() {
  int ret = ::listen(sockfd_, 1024);
  fatalif(ret<0, "TcpSocket listen error, errno %d", errno);
  info("TcpSocket %d has listened", sockfd_);
}

void TcpSocket::ShutdownWrite() {
  if (::shutdown(sockfd_, SHUT_WR)) {
    trace("TcpSocket %d Shutdown Write", sockfd_); 
  }
}

void TcpSocket::SetReuseAddr(bool on) {
  int optval = on ? 1 : 0;
  if (::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {
    fatal("TcpSocket %d set reuse address error, errno %d", sockfd_, errno);
  }
}

void TcpSocket::SetTcpNoDelay(bool on) {
  int optval = on ? 1 : 0;
  if (::setsockopt(sockfd_, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(optval)) < 0) {
    fatal("TcpSocket %d set tcp nodelay error, errno %d", sockfd_, errno); 
  }
}

InetAddress TcpSocket::GetLocalAddr() const {
  struct sockaddr_in localaddr;
  bzero(&localaddr, sizeof(localaddr));
  socklen_t addrlen = static_cast<socklen_t>(sizeof(localaddr));
  if (::getsockname(sockfd_, (struct sockaddr*)&localaddr, &addrlen) < 0) {
    error("TcpSocket %d getsockname error, errno %d", sockfd_, errno);
  }
  return InetAddress(localaddr);
}

InetAddress TcpSocket::GetPeerAddr() const {
  struct sockaddr_in peeraddr;
  bzero(&peeraddr, sizeof(peeraddr));
  socklen_t addrlen = static_cast<socklen_t>(sizeof(peeraddr));
  if (::getpeername(sockfd_, (struct sockaddr*)&peeraddr, &addrlen) < 0) {
    error("TcpSocket %d getpeername error, errno %d", sockfd_, errno);
  }
  return InetAddress(peeraddr);
}

int TcpSocket::Read(void* buf, int len) const {
  int nread = 0;
  while (nread < len) {
    int nr = ::read(sockfd_, static_cast<char*>(buf)+nread, len-nread);
    if (nr > 0) {
      nread += nr;
    } else if (nr == 0) { // EOF 
      break;
    } else if (errno != EINTR) {
      return -1;
    }
  }
  return nread;
}

int TcpSocket::Write(const void* buf, int len) const {
  int nwritten = 0;
  while (nwritten < len) {
    int nw = ::write(sockfd_, static_cast<const char*>(buf)+nwritten, len-nwritten);
    if (nw > 0) {
      nwritten += nw;
    } else if(nw == 0) { // EOF
      break; 
    } else if (errno != EINTR) {
      return -1;
    }
  }
  return nwritten;
}

TcpSocket TcpSocket::CreateTcpSocket() {
  int sockfd = ::socket(AF_INET, SOCK_STREAM|SOCK_CLOEXEC, IPPROTO_TCP);
  assert(sockfd > 0);
  return TcpSocket(sockfd);
}

std::unique_ptr<TcpSocket> TcpSocket::CreateTcpSocketPtr() {
  int sockfd = ::socket(AF_INET, SOCK_STREAM|SOCK_CLOEXEC, IPPROTO_TCP);
  assert(sockfd > 0);
  return std::unique_ptr<TcpSocket>(new TcpSocket(sockfd));
}

UdpSocket::UdpSocket(int sockfd)
  : Socket(sockfd) {
}

UdpSocket::UdpSocket(UdpSocket&& rhs)
  : Socket(std::move(rhs)) {
}

UdpSocket& UdpSocket::operator=(UdpSocket&& rhs) {
  std::swap(sockfd_, rhs.sockfd_);
  return *this;
}

int UdpSocket::Read(void* buf, int len) const {
  struct sockaddr_in peeraddr;
  socklen_t addrlen = static_cast<socklen_t>(sizeof(peeraddr));
  ssize_t nread = recvfrom(sockfd_, buf, len, 0, (struct sockaddr*)&peeraddr, &addrlen);
  InetAddress addr(peeraddr);
  debug("UdpSocket %d receive %d data from %s, data: %s", sockfd_, static_cast<int>(nread), addr.ToIpPort().data(), static_cast<char*>(buf));
  return nread;
}

int UdpSocket::Write(const void* buf, int len) const {
  (void)buf;
  (void)len;
  return 0;
}

UdpSocket UdpSocket::CreateUdpSocket() {
  int sockfd = ::socket(AF_INET, SOCK_DGRAM|SOCK_CLOEXEC, IPPROTO_UDP);
  assert(sockfd > 0);
  return UdpSocket(sockfd);
}

std::unique_ptr<UdpSocket> UdpSocket::CreateUdpSocketPtr() {
  int sockfd = ::socket(AF_INET, SOCK_DGRAM|SOCK_CLOEXEC, IPPROTO_UDP);
  assert(sockfd > 0);
  return std::unique_ptr<UdpSocket>(new UdpSocket(sockfd));
}
