#include "log.hpp"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/daily_file_sink.h"

void Log::initiate(std::string dir, unsigned short level)
{
  std::shared_ptr<spdlog::sinks::daily_file_format_sink_mt> fileSink = std::make_shared<spdlog::sinks::daily_file_format_sink_mt>(dir + "/timer.log", 0, 0);
  fileSink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");

#ifndef NDEBUG
  std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  consoleSink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");
#endif

  std::vector<spdlog::sink_ptr> sinks{
#ifndef NDEBUG
    consoleSink,
#endif
    fileSink
  };
  std::shared_ptr<spdlog::logger> logger = std::make_shared<spdlog::logger>("timer", sinks.begin(), sinks.end());
  if (level > spdlog::level::level_enum::err)
  {
    level = spdlog::level::level_enum::err;
  }
  logger->set_level(static_cast<spdlog::level::level_enum>(level));
  spdlog::set_default_logger(logger);
}
