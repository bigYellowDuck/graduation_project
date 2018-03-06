#include "logical_code.h"
#include "logging.h"
#include "socket.h"
#include "http_parser.h"
#include "event_loop.h"

#include <vector>
#include <string>

#include <rapidjson/document.h>

void LogicalCode::UdpLogicFunction(std::unique_ptr<char[]>&& data) {
  rapidjson::Document document;
  if (!document.Parse(data.get()).HasParseError()) {
    static const std::vector<std::string> params{"sensor_id", "sensor_type", "timestamp", "value"};
    for (const auto& param : params) {
      rapidjson::Value::ConstMemberIterator iter = document.FindMember(param.c_str());
      if (iter != document.MemberEnd()) {
        // TODO INSERT INTO MYSQL
        printf("%s\n", iter->value.GetString());
      } else {
        error("Json string lack param [%s], discard this json", param.c_str());
        return;
      }
    }
  } else {
    error("Receive a error or uncompleted json, discard it");
  }
}

void LogicalCode::TcpLogicFunction(struct ev_loop* loop, struct ev_io* watcher, int revents) {
  const TcpSocket *socket = static_cast<const TcpSocket*>(watcher->data);
  char buf[1024] = {0};
  int nread = socket->ReadOnce(buf, sizeof(buf));
  HttpParser parser(buf);
  std::string body = parser.GetBody();
  printf("%s\n", body.data());
  EventLoop* eloop = static_cast<EventLoop*>(ev_userdata(loop));
  eloop->RemoveSocket(watcher);
  (void)revents;
}
