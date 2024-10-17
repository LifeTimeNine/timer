#pragma once

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
}