#include "util.h"

namespace util {

int CreateEventFd() {
  int fd = eventfd(0, EFD_NONBLOCK|EFD_CLOEXEC);
  return fd;
}

pid_t GetThreadId() {
  return static_cast<pid_t>(::syscall(SYS_gettid));
}

} // namespace util
