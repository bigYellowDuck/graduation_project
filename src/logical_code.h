#ifndef LOGICAL_CODE_H
#define LOGICAL_CODE_H

#include <ev.h>
#include <memory>

class LogicalCode{
 public:
  static void UdpLogicFunction(std::unique_ptr<char[]>&& data);
  static void TcpLogicFunction(struct ev_loop* loop, struct ev_io* watcher, int revents);
};

#endif  // LOGICAL_CODE_H

