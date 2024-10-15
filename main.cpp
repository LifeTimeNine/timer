#include <iostream>
#include "cmdline.h"
#include "config.hpp"

Config* config = nullptr;

int main(int argc, char *argv[])
{
  cmdline::parser arg;
  arg.add<std::string>("config", 'c', "Config File", false, "/etc/timer.ini");
  arg.add<std::string>("host", 'h', "HTTP Host", false);
  arg.add<unsigned short>("port", 'p', "HTTP Port", false);
  arg.parse_check(argc, argv);

  config = new Config(arg.get<std::string>("config"));
  if (arg.exist("host")) config->setHttpHost(arg.get<std::string>("host"));
  if (arg.exist("port")) config->setHttpPort(arg.get<unsigned short>("port"));

  std::cout << "http host: " << config->getHttpHost() << std::endl
    << "http port: " << config->getHttpPort() << std::endl;

  delete config;

  return EXIT_SUCCESS;
}