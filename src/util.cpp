#include "util.hpp"
#include <fstream>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <mutex>
#include <filesystem>
#include <sys/stat.h>

namespace util
{
  std::mutex timeMutex;
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

  std::string getFormatTime(std::string format, date::sys_seconds* timePoint)
  {
    tm tm = getLocaltime(timePoint);
    std::stringstream result;
    result << std::put_time(&tm, format.data());
    return result.str();
  }

  std::string getFormatTime(std::string format)
  {
    date::sys_seconds timePoint = std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now());
    return getFormatTime(format, &timePoint);
  }

  tm getLocaltime()
  {
    std::lock_guard<std::mutex> lock(timeMutex);
    std::time_t now = time(0);
    return*std::localtime(&now);
  }

  tm getLocaltime(date::sys_seconds* timePoint)
  {
    auto now = std::chrono::system_clock::time_point(*timePoint);
    std::time_t timeT = std::chrono::system_clock::to_time_t(now);
    return *std::localtime(&timeT);
  }

  std::string fileGetContents(std::string filename)
  {
    std::ifstream file(filename);
    if (!file.is_open()) return "";
    std::string data;
    file >> data;
    file.close();
    return data;
  }

  bool filePutContents(std::string filename, std::string data)
  {
    if (!std::filesystem::exists(filename)) {
      size_t found = filename.find_last_of("/");
      std::string dir = filename.substr(0, found);
      struct stat info;
      if (stat(dir.data(), &info) != 0 || (info.st_mode & S_IFDIR) == 0 ) {
        // 创建目录
        std::string command = "mkdir -p " + dir;
        system(command.data());
      }
    }
    std::ofstream file(filename);
    if (!file.is_open()) return false;
    file << data;
    file.close();
    return true;
  }
}
