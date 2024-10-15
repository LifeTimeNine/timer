#include "log.hpp"
#include "spdlog/sinks/stdout_color_sinks.h"

spdlog::logger* Log::logger = nullptr;

void Log::initiate(Config* config)
{
  std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  consoleSink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");
  Log::logger = new spdlog::logger("timer", {consoleSink});
}

void Log::clear()
{
  if (Log::logger != nullptr) {
    delete Log::logger;
    Log::logger = nullptr;
  }
}