#include "cron.hpp"
#include <regex>
#include <set>
#include <cmath>
#include <ctime>
#include <iomanip>
#include "util.hpp"
#include "date/date.h"
#include "log.hpp"

Cron::Cron(
    unsigned long second,
    unsigned long minute,
    unsigned int hour,
    unsigned int day,
    unsigned short month,
    unsigned short week
  ): second(second),
  minute(minute),
  hour(hour),
  day(day),
  month(month),
  week(week) {
    secondRange = static_cast<unsigned long>(pow(2, 59)) - 1;
    minuteRange = static_cast<unsigned long>(pow(2, 59)) - 1;
    hourRange = pow(2, 23) - 1;
    dayRange = pow(2, 32) - 1;
    monthRange = pow(2, 13) - 1;
    weekRange = pow(2, 7) - 1;
}

unsigned long Cron::getSecond()
{
  return second;
}

unsigned long Cron::getMinute()
{
  return minute;
}

unsigned int Cron::getHour()
{
  return hour;
}

unsigned int Cron::getDay()
{
  return day;
}

unsigned short Cron::getMonth()
{
  return month;
}

unsigned short Cron::getWeek()
{
  return week;
}

bool Cron::checkExecute(unsigned short second, unsigned short minute, unsigned short hour, unsigned short day, unsigned short month, unsigned short week)
{
  unsigned long tmp = 1;
  return (getSecond() & (tmp << second)) > 0
    && (getMinute() & (tmp << minute)) > 0
    && (getHour() & (tmp << hour)) > 0
    && (getDay() & (tmp << day)) > 0
    && (getMonth() & (tmp << month)) > 0
    && (getWeek() & (tmp << week)) > 0;
}

Cron Cron::parse(std::string cron)
{
  std::regex reg("(((\\d{1,2}-\\d{1,2})(\\/\\d{1,2})?|\\d{1,2}|\\*(\\/\\d{1,2})?|\\?),?)+");
  std::smatch match;
  std::vector<std::string> itemList;
  std::string cronBak = cron;
  while (std::regex_search(cron, match, reg))
  {
    itemList.push_back(match[0].str());
    cron = match.suffix().str();
  }
  
  if (itemList.size() != 5 && itemList.size() != 6) throw std::invalid_argument("The Cron format is incorrect: " + cronBak);
  // 如果输入的是分钟级别的Cron，秒默认是 0
  if (itemList.size() == 5) itemList.insert(itemList.begin(), "0");
  return Cron(
    parseItem(itemList[0], 0, 59),
    parseItem(itemList[1], 0, 59),
    parseItem(itemList[2], 0, 23),
    parseItem(itemList[3], 1, 31),
    parseItem(itemList[4], 1, 12),
    parseItem(itemList[5], 0, 6)
  );
}

unsigned long Cron::parseItem(std::string cronItem, unsigned short min, unsigned short max)
{
  std::set<unsigned short> set;
  for(std::string item : util::stringSplit(cronItem, ',')) {
    std::regex reg("^((\\d{1,2}-\\d{1,2})(\\/(\\d{1,2}))?|\\d{1,2}|(\\*)(\\/(\\d{1,2}))?|\\?)$");
    std::smatch match;
    std::regex_search(item, match, reg);
    unsigned short begin = 0, end = 0, step = 1;
    if (!match[5].str().empty() || match[1].str() == "?") {
      begin = min;
      end = max;
      step = match[7].str().empty() ? 1 : std::stoi(match[7].str());
    } else if (!match[2].str().empty()) {
      std::vector<std::string> beginEndStr = util::stringSplit(match[2].str(), '-');
      begin = std::stoi(beginEndStr[0]);
      end = std::stoi(beginEndStr[1]);
      step = match[4].str().empty() ? 1 : std::stoi(match[4].str());
    } else {
      begin = std::stoi(match[1].str());
      end = std::stoi(match[1].str());
    }
    if (begin < min) begin = min;
    if (end > max) end = max;
    for(unsigned short i = begin; i <= end; i += step) {
      set.insert(i);
    }
  }
  unsigned long result = 0;
  for(unsigned short item: set) {
    result |= static_cast<unsigned long>(1) << item;
  }
  return result;
}

std::chrono::_V2::system_clock::time_point Cron::getNextRunTime(std::chrono::_V2::system_clock::time_point time)
{
  std::chrono::seconds currentTime = std::chrono::duration_cast<std::chrono::seconds>(time.time_since_epoch());
  std::chrono::seconds second(1);
  currentTime += second;
  calculateNextRunMonth(currentTime);
  std::time_t t = currentTime.count();
  std::tm* timeInfo = std::localtime(&t);
  std::ostringstream oss;
  oss << std::put_time(timeInfo, "%Y-%m-%d %H:%M:%S");
  std::cout << oss.str() << std::endl;
  return time;
}

void Cron::calculateNextRunMonth(std::chrono::seconds &time)
{
  tm* tm = getTime(&time);
  unsigned short current = tm->tm_mon + 1;
  Log::debug("month {}-{}-{} {}:{}:{}", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
  unsigned short result = getMonth() & (monthRange << current);
  if (result == 0) {
    // 时间调整到下一年第一个月第一天的0点0分0秒
    date::year_month_day nextYearDate{date::year{tm->tm_year + 1900} + date::years{1}, date::month{1}, date::day{1}};
    date::sys_days ymd = nextYearDate;
    auto nextYearFirstSecond = ymd - std::chrono::hours{8};
    time = std::chrono::time_point_cast<std::chrono::seconds>(nextYearFirstSecond).time_since_epoch();
    calculateNextRunMonth(time);
  } else {
    // 计算下一次满足条件的月份
    unsigned short next = 0;
    unsigned short tmp = 1;
    for (size_t i = 1; i <= 12; i++)
    {
      if ((result & (tmp << i)) != 0) {
        next = i;
        break;
      }
    }
    if (next != current) {
      date::months incMonth(next - current);
      time += incMonth;
    }
    if (week == weekRange) {
      calculateNextRunDay(time);
    } else {
      calculateNextRunWeek(time);
    }
  }
}

void Cron::calculateNextRunDay(std::chrono::seconds &time)
{
  tm* tm = getTime(&time);
  unsigned short current = tm->tm_mday;
  Log::debug("day {}-{}-{} {}:{}:{}", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
  unsigned int result = getDay() & (dayRange << current);
  if (result == 0) {
    // 时间调整到下一个月第一天的0点0分0秒
    date::year_month_day nextMonthDate = {date::year{tm->tm_year + 1900}, date::month{static_cast<unsigned int>(tm->tm_mon + 1)} + date::months{1}, date::day{1}};
    date::sys_days ymd = nextMonthDate;
    auto nextYearFirstSecond = ymd - std::chrono::hours{8};
    time = std::chrono::time_point_cast<std::chrono::seconds>(nextYearFirstSecond).time_since_epoch();
    calculateNextRunMonth(time) ;
  } else {
    // 计算下一次满足条件的天
    unsigned short next = 0;
    unsigned int tmp = 1;
    for (size_t i = 1; i <= 31; i++)
    {
      if ((result & (tmp << i)) != 0) {
        next = i;
        break;
      }
    }
    if (next != current) {
      date::days incDay(next - current);
      time += incDay;
    }
    calculateNextRunHour(time);
  }
}

void Cron::calculateNextRunHour(std::chrono::seconds &time)
{
  tm* tm = getTime(&time);
  unsigned short current = tm->tm_hour;
  Log::debug("hour {}-{}-{} {}:{}:{}", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
  unsigned int result = getHour() & (hourRange << current);
  if (result == 0) {
    // 时间调整到明天的0点0分0秒
    date::year_month_day tomorrow = {date::year{tm->tm_year + 1900}, date::month{static_cast<unsigned int>(tm->tm_mon)}, date::day{static_cast<unsigned int>(tm->tm_mday)} + date::days{1}};
    date::sys_days ymd = tomorrow;
    auto nextYearFirstSecond = ymd - std::chrono::hours{8};
    time = std::chrono::time_point_cast<std::chrono::seconds>(nextYearFirstSecond).time_since_epoch();
    calculateNextRunDay(time);
  } else {
    // 计算下一次满足条件的小时
    unsigned short next = 0;
    unsigned int tmp = 1;
    for (size_t i = 0; i <= 23; i++)
    {
      if ((result & (tmp << i)) != 0) {
        next = i;
        break;
      }
    }
    if (next != current) {
      std::chrono::hours incHour(next - current);
      time += incHour;
    }
    calculateNextRunMinute(time);
  }
}

void Cron::calculateNextRunMinute(std::chrono::seconds &time)
{
  tm* tm = getTime(&time);
  unsigned short current = tm->tm_min;
  Log::debug("minute {}-{}-{} {}:{}:{}", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
  unsigned long result = getMinute() & (minuteRange << current);
  if (result == 0) {
    // 时间调整到下一小时的0分0秒
    std::chrono::hours incHour(1);
    std::chrono::minutes decMinute(tm->tm_min);
    std::chrono::seconds decSecond(tm->tm_sec);
    time = time + incHour - decMinute - decSecond;
    calculateNextRunHour(time);
  } else {
    // 计算下一次满足条件的分钟
    unsigned short next = 0;
    unsigned long tmp = 1;
    for (size_t i = 0; i <= 59; i++)
    {
      if ((result & (tmp << i)) != 0) {
        next = i;
        break;
      }
    }
    if (next != current) {
      std::chrono::minutes incMinute(next - current);
      time += incMinute;
    }
    calculateNextRunSecond(time);
  }
}

void Cron::calculateNextRunSecond(std::chrono::seconds &time)
{
  tm* tm = getTime(&time);
  unsigned short current = tm->tm_sec;
  Log::debug("second {}-{}-{} {}:{}:{}", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
  unsigned long result = getSecond() & (secondRange << current);
  if (result == 0) {
    // 时间调整到下一分种的0秒
    std::chrono::minutes incMinute(1);
    std::chrono::seconds decSecond(tm->tm_sec);
    time = time + incMinute - decSecond;
    calculateNextRunMinute(time);
  } else {
    // 计算下一次满足条件的秒
    unsigned short next = 0;
    unsigned long tmp = 1;
    for (size_t i = 0; i <= 59; i++)
    {
      if ((result & (tmp << i)) != 0) {
        next = i;
        break;
      }
    }
    if (next != current) {
      std::chrono::seconds incSecond(next - current);
      time += incSecond;
    }
  }
}

void Cron::calculateNextRunWeek(std::chrono::seconds &time)
{
  Log::debug("week");
  tm* tm = getTime(&time);
  unsigned short current = tm->tm_wday;
  unsigned long result = getWeek() & (weekRange << current);
  if (result == 0) {
    result = weekRange;
  } else {
    // 计算下一次满足条件的周
    unsigned short next = 0;
    unsigned short tmp = 1;
    for (size_t i = 0; i <= 59; i++)
    {
      if ((result & (tmp << i)) != 0) {
        next = i;
        break;
      }
    }
    if (next != current) {
      date::days incDay(next - current);
      time += incDay;
    }
    calculateNextRunHour(time);
  }
}

tm* Cron::getTime(const std::chrono::seconds* time)
{
  auto timePoint = std::chrono::system_clock::time_point(*time);
  std::time_t timeT = std::chrono::system_clock::to_time_t(timePoint);
  return std::localtime(&timeT);
}
