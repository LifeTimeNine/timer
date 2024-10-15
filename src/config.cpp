#include "config.hpp"
#include "SimpleIni.h"

Config::Config(std::string file)
{
  CSimpleIniA ini;
  if (ini.LoadFile(file.data()) == SI_OK) {
    httpHost = ini.GetValue("http", "host");
    httpPort = ini.GetLongValue("http", "port");
  }
}

Config Config::setHttpHost(std::string host)
{
  httpHost = host;
  return *this;
}
std::string Config::getHttpHost()
{
  return httpHost;
}

Config Config::setHttpPort(unsigned short port)
{
  httpPort = port;
  return *this;
}
unsigned short Config::getHttpPort()
{
  return httpPort;
}