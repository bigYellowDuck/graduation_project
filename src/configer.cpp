#include "configer.h"
#include "tinyxml2.h"
#include "logging.h"

#include <stdlib.h>

using namespace tinyxml2;

Configer::Configer(const std::string& conf_path) {
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

