#ifndef LOGICAL_CODE_H
#define LOGICAL_CODE_H

#include <ev.h>
#include <memory>

#include "db_connector.h"

class LogicalCode{
 public:
  static void UdpLogicFunction(std::unique_ptr<char[]>&& data, DbConnector&);
  static void TcpLogicFunction(struct ev_loop* loop, struct ev_io* watcher, int revents);
};

#endif  // LOGICAL_CODE_H

