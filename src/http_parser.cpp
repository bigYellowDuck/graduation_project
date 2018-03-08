#include "http_parser.h"

#include <string.h>

HttpParser::HttpParser(const char* data)
  : data_(data) {
}

HttpParser::~HttpParser() {
  data_ = NULL;
}

std::string HttpParser::GetBody() {
  const char* p = strstr(data_, "\r\n\r\n");
  if (p == NULL) {
    return std::string();
  }
  std::string str(p+4);
  return str;
}

std::string HttpParser::GetRequestParam() {
  const char* p = strstr(data_,"\r\n");
  std::string request_line(data_, p-data_);
  auto pos1 = request_line.find("?");
  if (pos1 == std::string::npos) {
    return std::string();
  }

  auto pos2 = request_line.rfind(" ");
  if (pos2 == std::string::npos) {
    return std::string();
  }

  std::string params(request_line.begin()+pos1+1, request_line.begin()+pos2);

  return params;
}
