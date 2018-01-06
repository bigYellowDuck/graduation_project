#include "tinyxml2.h"

#include <stdio.h>
#include <string>
#include <iostream>

using namespace tinyxml2;

int main() {
  XMLDocument doc;
  doc.LoadFile("server.xml");
  XMLElement* root = doc.RootElement();
  std::string threadnum = root->FirstChildElement("Threads")->Attribute("num");
  std::cout << threadnum << std::endl;
  std::string tcpport = root->FirstChildElement("Tcp")->Attribute("port");
  std::cout << tcpport << std::endl;
  std::string udpport = root->FirstChildElement("Udp")->Attribute("port");
  std::cout << udpport << std::endl;
  std::string logfile = root->FirstChildElement("Log")->Attribute("file");
  std::cout << logfile << std::endl;
  std::string loglevel = root->FirstChildElement("Log")->Attribute("level");
  std::cout << loglevel << std::endl;

  

  return 0;
}
