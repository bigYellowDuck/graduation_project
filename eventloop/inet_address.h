#ifndef INET_ADDRESS_H_
#define INET_ADDRESS_H_

#include <netinet/in.h>
#include <string>

class InetAddress
{
 public:
  explicit InetAddress(const std::string &ipPort);  // "1.2.3.4:5678"
  InetAddress(const std::string &ip, uint16_t port);
  InetAddress(uint16_t port, bool loopbackOnly = false);
    
  explicit InetAddress(const struct sockaddr_in &saddr) 
   : saddr_(saddr) { 
  }

  // default copy ctor/assignment, move ctor/assignment dtor are Okay  
  
  std::string ToIp() const;
  std::string ToIpPort() const;

  const struct sockaddr_in& GetSockAddrInet() const { return saddr_; }
  void SetSockAddrInet(const struct sockaddr_in &saddr) { saddr_ = saddr; }

  void SetPort(uint16_t port) { saddr_.sin_port = htons(port); }
    
  uint32_t IpNetEndian() const { return saddr_.sin_addr.s_addr; }
  uint16_t PortNetEndian() const { return saddr_.sin_port; }

  uint32_t IpHostEndian() const { return ntohl(saddr_.sin_addr.s_addr); }
  uint16_t PortHostEndian() const { return ntohs(saddr_.sin_port); }
    
  bool operator==(const InetAddress &rhs) const;
 private:
  void HelpConsturct(const char *ip, uint16_t port);
  struct sockaddr_in saddr_; 
};

#endif  // INET_ADDRESS_H_

