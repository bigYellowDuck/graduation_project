#include "logical_code.h"
#include "logging.h"
#include "socket.h"
#include "http_parser.h"
#include "event_loop.h"
#include "util.h"

#include <vector>
#include <string>
#include <map>

#include <rapidjson/document.h>

void LogicalCode::UdpLogicFunction(std::unique_ptr<char[]>&& data, DbConnector& connector) {
  rapidjson::Document document;
  if (!document.Parse(data.get()).HasParseError()) {
    static const std::vector<std::string> params{"sensor_id", "sensor_type", "timestamp", "value"};
    std::map<std::string, std::string> paramskv;
    for (const auto& param : params) {
      rapidjson::Value::ConstMemberIterator iter = document.FindMember(param.c_str());
      if (iter != document.MemberEnd()) {
        paramskv[param] = iter->value.GetString();
      } else {
        error("Json string lack param [%s], discard this json", param.c_str());
        return;
      }
    }
    char sql[256];
    sprintf(sql, "insert into sensors(%s, %s, %s, %s) values('%s', '%s', '%s', '%s')", 
            params[0].c_str(), params[1].c_str(), params[2].c_str(), params[3].c_str(),
            paramskv[params[0]].c_str(), paramskv[params[1]].c_str(), 
            paramskv[params[2]].c_str(), paramskv[params[3]].c_str());
    bool state = connector.ExecInsertSql(sql);
    if (state) {
      info("Exec %s successfully", sql);
    } else {
      info("Exec %s failed", sql);
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
  std::string request_line_param = parser.GetRequestParam();
  
  std::map<std::string, std::string> record;
  util::StrToMap(request_line_param, record);
  
  static const std::vector<std::string> params{"sensor_id", "sensor_type", "timestamp"};
  for (const auto& param : params) {
    if (record.find(param) == record.end()) {
      error("HTTP request line lack param [%s]", param.c_str());
      return;
    }
  }
  char sql[256];
  sprintf(sql, "select value from sensors where %s='%s' and %s='%s' and %s='%s'",
          params[0].c_str(), record[params[0]].c_str(),
          params[1].c_str(), record[params[1]].c_str(),
          params[2].c_str(), record[params[2]].c_str());

  EventLoop* eloop = static_cast<EventLoop*>(ev_userdata(loop));

  auto select_res = (eloop->GetDbConnector()).ExecSelectSql(sql);
  info("Exec %s successfully", sql);
  if (!select_res.empty()) {
    // TODO response the value
    // printf("%s\n", select_res[0][0].c_str());
    char data[1024] = {0};
    sprintf(data, "HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\nAccess-Control-Allow-Origin:*\r\n\r\n%s\r\n", 
            select_res[0][0].c_str());

    socket->Write(data, strlen(data));
  } else {
    info("Select return null");
  }
  
  eloop->RemoveSocket(watcher);
  (void)revents;
}
