#include "util.hpp"

#include <sstream>
#include <ctime>
#include <iomanip>

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

  unsigned long getTimestamp()
  {
    return std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now()).time_since_epoch().count();
  }

  unsigned long getMillisecondTimestamp()
  {
    return std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()).time_since_epoch().count();
  }

  std::string getFormatTime(std::string format, std::chrono::_V2::system_clock::time_point timePoint)
  {
    std::time_t time = std::chrono::system_clock::to_time_t(timePoint);
    tm* localTm = std::localtime(&time);

    std::stringstream result;
    result << std::put_time(std::localtime(&time), format.data());
    return result.str();
  }

  std::string getFormatTime(std::string format)
  {
    std::chrono::_V2::system_clock::time_point now = std::chrono::system_clock::now();
    return getFormatTime(format, now);
  }
}
