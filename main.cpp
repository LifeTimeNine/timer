#include <iostream>
#include "cmdline.h"
#include "config.hpp"
#include "log.hpp"

Config* config = nullptr;

int main(int argc, char *argv[])
{
  // 解析参数
  cmdline::parser arg;
  arg.add<std::string>("config", 'c', "Config File", false, "/etc/timer.ini");
  arg.add<std::string>("host", 'h', "HTTP Host", false);
  arg.add<unsigned short>("port", 'p', "HTTP Port", false);
  arg.parse_check(argc, argv);

  // 初始化配置类
  config = new Config(arg.get<std::string>("config"));
  if (arg.exist("host")) config->setHttpHost(arg.get<std::string>("host"));
  if (arg.exist("port")) config->setHttpPort(arg.get<unsigned short>("port"));

  // 初始化日志模块
  Log::initiate(config->getLogDir(), config->getLogLevel());

  delete config;

  return EXIT_SUCCESS;
}