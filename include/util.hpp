#pragma once

#include <chrono>
#include <iostream>
#include <vector>

namespace util
{
  /** 
   * 字符串分割
   * @param str       字符串
   * @param delimiter 分割符
   */
  std::vector<std::string> stringSplit(std::string str, char delimiter);

  /**
   * 获取时间戳
   */
  unsigned long getTimestamp();

  /**
   * 获取当前毫秒时间戳
   */
  unsigned long getMillisecondTimestamp();

  /**
   * 获取格式化的日期
   * @param format      要格式化的格式
   * @param timePoint   指定时间点
   */
  std::string getFormatTime(std::string format, std::chrono::_V2::system_clock::time_point timePoint);

  /**
   * 获取格式化的日期
   * @param format  要格式化的格式
   */
  std::string getFormatTime(std::string format);
}