#include "http_parser.h"

#include <string.h>

HttpParser::HttpParser(const char* data)
  : data_(data) {
}

HttpParser::~HttpParser() {
  data_ = NULL;
}

std::string HttpParser::GetBody() {
  const char *p = strstr(data_, "\r\n\r\n");
  if (p == NULL) {
    return std::string();
  }
  std::string str(p+4);
  return str;
}
