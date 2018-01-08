#include "server.h"
#include "util.h"

#include <iostream>
#include <stdio.h>
#include <unistd.h>


/*
static void TimerCallback(struct ev_loop* l, struct ev_timer* watcher, int revents) {
  printf("%u timercallback\n", util::GetThreadId());
}
*/

static void PrintUdpData(std::unique_ptr<char[]>&& data) {
  printf("%d data %s\n", util::GetThreadId(), data.get());
}

int main(int argc, char** argv) {
  if (argc != 2) {
    fprintf(stderr, "useage: <%s> <config file path>\n", argv[0]);
    return -1;
  }

  Server server(argv[1]);
  server.SetUdpCallback(PrintUdpData);
  server.Run();
/*
  Configer configer(argv[1]);
  
  setlogfile(configer.LogfileName());
  setloglevel(configer.Loglevel());

  EventLoop el;
  MultiEventLoop loop(&el, configer.ThreadNum());

  InetAddress addr(configer.TcpPort());
  Acceptor acceptor(addr, &el); 

  InetAddress udpaddr(configer.UdpPort());
  Udpkeeper udpkeeper(udpaddr, &el);
  Udpkeeper::setReadCallBack(PrintUdpData);

  trace("multiloop start");

  loop.Start();
*/
/*
  loop.GetNextLoop()->AddTimerEvent(2, 0, TimerCallback);
*/
  return 0;
}
