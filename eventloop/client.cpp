#include "socket.h"
#include "inet_address.h"
#include <string>
int main() {
  InetAddress addr("127.0.0.1:9999");
  TcpSocket socket = TcpSocket::CreateTcpSocket();
  socket.Connect(addr);


  std::string str("hello, world");
  socket.Write(str.data(), str.size());
  socket.ShutdownWrite();
  
  char buf[1024] = {0};
  int nread = socket.Read(buf, sizeof(buf));
  printf("%d %s\n",nread, buf);
  return 0;
}
