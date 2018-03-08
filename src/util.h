#ifndef UTIL_H_
#define UTIL_H_

#include <sys/eventfd.h>
#include <sys/syscall.h>
#include <unistd.h>

#include <string>
#include <map>

namespace util {

int CreateEventFd();

pid_t GetThreadId();

int GetField(std::string& str, std::string& value);

int GetFieldValue(const std::string& str, std::string& name, std::string& value);

void StrToMap(std::string& buf, std::map<std::string, std::string>& record);

} // namespace util


#endif  // UTIL_H_
