#ifndef UTIL_H_
#define UTIL_H_

#include <sys/eventfd.h>
#include <sys/syscall.h>
#include <unistd.h>

namespace util {

int CreateEventFd();


pid_t GetThreadId();

} // namespace util


#endif  // UTIL_H_
