#include "configer.h"
#include "server.h"
#include "logical_code.h"

#include <stdio.h>


int main(int argc, char** argv) {
  if (argc != 2) {
    fprintf(stderr, "useage: <%s> <config file path>\n", argv[0]);
    return -1;
  }
  
  static Configer& configer = Configer::Instance();
  configer.init(argv[1]); 

  Server server;
  server.SetUdpCallback(LogicalCode::UdpLogicFunction);
  server.SetTcpCallback(LogicalCode::TcpLogicFunction);
  server.Run();
/*
  loop.GetNextLoop()->AddTimerEvent(2, 0, TimerCallback);
*/
  return 0;
}
