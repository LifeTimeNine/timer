#include "util.hpp"

#include <sstream>

namespace util
{
  /** 
   * 字符串分割
   * @param str       字符串
   * @param delimiter 分割符
   */
  std::vector<std::string> stringSplit(std::string str, char delimiter)
  {
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string item;
    while (std::getline(ss, item, delimiter))
    {
      if (!item.empty()) {
        result.push_back(item);
      }
    }
    return result;
  }
}
