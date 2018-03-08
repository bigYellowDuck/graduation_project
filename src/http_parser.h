#ifndef HTTP_PARSER_H_
#define HTTP_PARSER_H_

#include <string>

class HttpParser {
 public:
  HttpParser(const char* data);
  ~HttpParser();

  HttpParser(const HttpParser&) = delete;
  HttpParser& operator=(const HttpParser&) = delete;

  std::string GetBody();
  std::string GetRequestParam();
 private:
  const char* data_;
};

#endif  // HTTP_PARSER_H_

