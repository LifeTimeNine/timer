#include "config.hpp"
#include "SimpleIni.h"

Config::Config(std::string file)
{
  CSimpleIniA ini;
  if (ini.LoadFile(file.data()) == SI_OK) {
    httpHost = ini.GetValue("http", "host", "127.0.0.1");
    httpPort = ini.GetLongValue("http", "port", 10010);
    logDir = ini.GetValue("log", "dir", "/var/log/timer");
    logLevel = ini.GetLongValue("log", "level", 0);
    dbPath =ini.GetValue("db", "path", "/var/lib/timer/tas.db");
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

std::string Config::getLogDir()
{
  return logDir;
}
unsigned short Config::getLogLevel()
{
  return logLevel;
}
std::string Config::getDbPath()
{
  return dbPath;
}