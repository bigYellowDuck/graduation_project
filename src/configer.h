#ifndef CONFIGER_H_
#define CONFIGER_H_

#include <string>

class Configer {
 public:
  ~Configer() = default;

  Configer(const Configer&) = delete;
  Configer& operator=(const Configer&) = delete;

  void init(const std::string& conf_path);

  int ThreadNum() const;
  int TcpPort() const;
  int UdpPort() const;
  const std::string LogfileName() const;
  const std::string Loglevel() const;

  static Configer& Instance();

 private:
  Configer() = default;

  std::string thread_num_;
  std::string tcp_port_;
  std::string udp_port_;
  std::string logfile_name_;
  std::string loglevel_;

  static Configer instance_;
};

#endif //  CONFIGER_H_

