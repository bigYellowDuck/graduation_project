#include "event_loop.h"
#include "util.h"
#include "inet_address.h"
#include "socket.h"
#include "acceptor.h"
#include "logging.h"
#include "udpkeeper.h"
#include "tinyxml2.h"

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

using namespace tinyxml2;
int main() {

  XMLDocument doc;
  doc.LoadFile("conf/server.xml");
  XMLElement* root = doc.RootElement();
  std::string threadnum = root->FirstChildElement("Threads")->Attribute("num");
  std::cout << threadnum << std::endl;
  std::string tcpport = root->FirstChildElement("Tcp")->Attribute("port");
  std::cout << tcpport << std::endl;
  std::string udpport = root->FirstChildElement("Udp")->Attribute("port");
  std::cout << udpport << std::endl;
  std::string logfile = root->FirstChildElement("Log")->Attribute("file");
  std::cout << logfile << std::endl;
  std::string loglevel = root->FirstChildElement("Log")->Attribute("level");
  std::cout << loglevel << std::endl;

  setloglevel("TRACE");
  setlogfile("test.log");
  
  EventLoop el;
  MultiEventLoop loop(&el, 3);

  InetAddress addr(9999);
  Acceptor acceptor(addr, &el); 

  InetAddress udpaddr(19999);
  Udpkeeper udpkeeper(udpaddr, &el);
  Udpkeeper::setReadCallBack(PrintUdpData);

  loop.Start();
/*
  loop.GetNextLoop()->AddTimerEvent(2, 0, TimerCallback);
*/
  return 0;
}
