#include "inet_address.h"

#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

InetAddress::InetAddress(const std::string &ipPort) {
  auto colon = ipPort.find(':');
  if (colon == std::string::npos)
  {
    fprintf(stderr, "string ipPort error");
    exit(1);
  }

  uint16_t port = static_cast<uint16_t>(::atoi(std::string(ipPort.begin()+colon+1, ipPort.end()).data()));
  HelpConsturct(std::string(ipPort.begin(), ipPort.begin()+colon).data(), port);
    
}

InetAddress::InetAddress(const std::string &ip, uint16_t port) {
  HelpConsturct(ip.data(), port);
}

InetAddress::InetAddress(uint16_t port, bool loopbackOnly) {
  ::bzero(&saddr_, sizeof(saddr_));
  saddr_.sin_family = AF_INET;
  saddr_.sin_addr.s_addr = htonl(loopbackOnly ? INADDR_LOOPBACK : INADDR_ANY);
  saddr_.sin_port = htons(port);
}

std::string InetAddress::ToIp() const {
  char buf[32] = "";
  ::inet_ntop(AF_INET, &saddr_.sin_addr, buf, sizeof(buf));
  return buf;
}

std::string InetAddress::ToIpPort() const {
  char buf[32] = ""; // "255.255.255.255:65536" = 4*4+5+1 = 22
  ::inet_ntop(AF_INET, &saddr_.sin_addr, buf, sizeof(buf));
  int end = ::strlen(buf);
  uint16_t port = PortHostEndian();
  snprintf(buf+end, sizeof(buf) - end, ":%u", port);
  return buf;
}

bool InetAddress::operator==(const InetAddress &rhs) const {
  return saddr_.sin_family == rhs.saddr_.sin_family
    && IpNetEndian() == rhs.IpNetEndian()
    && PortNetEndian() == rhs.PortNetEndian();
}

void InetAddress::HelpConsturct(const char *ip, uint16_t port) {
  ::bzero(&saddr_, sizeof(saddr_));
  saddr_.sin_family = AF_INET;
  ::inet_pton(AF_INET, ip, &saddr_.sin_addr);
  saddr_.sin_port = htons(port);
}

