#ifndef HTTP_PARSER_H_
#define HTTP_PARSER_H_

#include <string>

class HttpParser {
 public:
  HttpParser(const char* data);
  ~HttpParser();

  std::string GetBody();

 private:
  const char* data_;
};

#endif  // HTTP_PARSER_H_

