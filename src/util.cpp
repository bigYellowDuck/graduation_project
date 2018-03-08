#include "util.h"

using std::string;
using std::map;

namespace util {

int CreateEventFd() {
  int fd = eventfd(0, EFD_NONBLOCK|EFD_CLOEXEC);
  return fd;
}

pid_t GetThreadId() {
  return static_cast<pid_t>(::syscall(SYS_gettid));
}

int GetField(string& str, string& value) {
  if (str.empty()) {
    return 0;
  }
  
  auto start = str.find("&");
  if (start == string::npos || start != 0) {
    start = 0;
  } else {
    start += 1;
  }

  auto end = str.find("&", start);
  if (end == string::npos) {
    end = str.size();
  }

  value = str.substr(start, end-start);
  str = str.substr(end);

  return 1;
}

int GetFieldValue(const string& str, string& name, string& value) {
  auto pos = str.find("=");
  if (pos == string::npos) {
    return 0;
  }

  name = str.substr(0, pos);
  value = str.substr(pos+1);

  return 1;
}

void StrToMap(string& buf, map<string, string>& record) {
  string bvalue, fname, fvalue;
  string stbuf = buf;

  while (GetField(stbuf, bvalue)) {
    if (!GetFieldValue(bvalue, fname, fvalue)) {
      continue;
    }
    record[fname] = fvalue;
  }

}

} // namespace util
