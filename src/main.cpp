#include "server.h"
#include "util.h"
#include "http_parser.h"
#include "event_loop.h"
#include "socket.h"
#include "logging.h"

#include <rapidjson/document.h>

#include <stdio.h>
#include <unistd.h>
#include <vector>

static void PrintUdpData(std::unique_ptr<char[]>&& data) {
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

static void PrintTcpData(struct ev_loop* loop, struct ev_io* watcher, int revents) {
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


int main(int argc, char** argv) {
  if (argc != 2) {
    fprintf(stderr, "useage: <%s> <config file path>\n", argv[0]);
    return -1;
  }

  Server server(argv[1]);
  server.SetUdpCallback(PrintUdpData);
  server.SetTcpCallback(PrintTcpData);
  server.Run();
/*
  loop.GetNextLoop()->AddTimerEvent(2, 0, TimerCallback);
*/
  return 0;
}
