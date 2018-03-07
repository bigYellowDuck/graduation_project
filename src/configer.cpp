#include "configer.h"
#include "tinyxml2.h"
#include "logging.h"

#include <stdlib.h>

using namespace tinyxml2;

Configer Configer::instance_;

Configer& Configer::Instance() {
  return instance_;
}

void Configer::init(const std::string& conf_path) {
  XMLDocument doc;
  doc.LoadFile(conf_path.data());
  XMLElement* root = doc.RootElement();
  thread_num_ = root->FirstChildElement("Threads")->Attribute("num");
  tcp_port_ = root->FirstChildElement("Tcp")->Attribute("port");
  udp_port_ = root->FirstChildElement("Udp")->Attribute("port");
  logfile_name_ = root->FirstChildElement("Log")->Attribute("file");
  loglevel_ = root->FirstChildElement("Log")->Attribute("level");
  setlogfile(logfile_name_);
  setloglevel(loglevel_);

  db_host_ = root->FirstChildElement("Db")->Attribute("host");
  db_user_ = root->FirstChildElement("Db")->Attribute("user");
  db_password_ = root->FirstChildElement("Db")->Attribute("password");
  db_name_ = root->FirstChildElement("Db")->Attribute("dbname");
}

int Configer::ThreadNum() const {
  return atoi(thread_num_.data()); 
}

int Configer::TcpPort() const {
  return atoi(tcp_port_.data());
}

int Configer::UdpPort() const {
  return atoi(udp_port_.data());
}

const std::string Configer::LogfileName() const {
  return logfile_name_;
}

const std::string Configer::Loglevel() const {
  return loglevel_;
}

const std::string Configer::DbHost() const {
  return db_host_;
}

const std::string Configer::DbUser() const {
  return db_user_;
}

const std::string Configer::DbPassword() const {
  return db_password_;
}

const std::string Configer::DbName() const {
  return db_name_;
}
