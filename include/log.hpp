#pragma once
#include "config.hpp"
#include "spdlog/spdlog.h"
#include "spdlog/logger.h"

namespace Log
{
  /**
   * 日志初始化
   * @param dir   日志目录
   * @param level 日志等级
   */
  void initiate(std::string dir, unsigned short level);

  template <typename... Args>
  void trace(fmt::format_string<Args...> fmt, Args &&...args)
  {
    spdlog::default_logger()->log(spdlog::level::level_enum::trace, fmt, std::forward<Args>(args)...);
  }

  template <typename... Args>
  void debug(fmt::format_string<Args...> fmt, Args &&...args) {
    spdlog::default_logger()->log(spdlog::level::level_enum::debug, fmt, std::forward<Args>(args)...);
  }

  template <typename... Args>
  void info(fmt::format_string<Args...> fmt, Args &&...args) {
    spdlog::default_logger()->log(spdlog::level::level_enum::info, fmt, std::forward<Args>(args)...);
  }

  template <typename... Args>
  void warn(fmt::format_string<Args...> fmt, Args &&...args) {
    spdlog::default_logger()->log(spdlog::level::level_enum::warn, fmt, std::forward<Args>(args)...);
  }

  template <typename... Args>
  void error(fmt::format_string<Args...> fmt, Args &&...args) {
    spdlog::default_logger()->log(spdlog::level::level_enum::err, fmt, std::forward<Args>(args)...);
  }

  template <typename... Args>
  void critical(fmt::format_string<Args...> fmt, Args &&...args) {
    spdlog::default_logger()->log(spdlog::level::level_enum::critical, fmt, std::forward<Args>(args)...);
  }
}
