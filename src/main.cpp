#include "server.h"
#include "logical_code.h"

#include <stdio.h>

int main(int argc, char** argv) {
  if (argc != 2) {
    fprintf(stderr, "useage: <%s> <config file path>\n", argv[0]);
    return -1;
  }

  Server server(argv[1]);
  server.SetUdpCallback(LogicalCode::UdpLogicFunction);
  server.SetTcpCallback(LogicalCode::TcpLogicFunction);
  server.Run();
/*
  loop.GetNextLoop()->AddTimerEvent(2, 0, TimerCallback);
*/
  return 0;
}
