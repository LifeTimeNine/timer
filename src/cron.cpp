#include "cron.hpp"
#include <regex>
#include <set>
#include "util.hpp"

Cron::Cron(
    unsigned long minuteRange,
    unsigned int hourRange,
    unsigned int dayRange,
    unsigned short monthRange,
    unsigned short weekRange
  ): minuteRange(minuteRange),
  hourRange(hourRange),
  dayRange(dayRange),
  monthRange(monthRange),
  weekRange(weekRange) {}

unsigned long Cron::getMinuteRange()
{
  return minuteRange;
}

unsigned int Cron::getHourRange()
{
  return hourRange;
}

unsigned int Cron::getDayRange()
{
  return dayRange;
}

unsigned short Cron::getMonthRange()
{
  return monthRange;
}

unsigned short Cron::getWeekRange()
{
  return weekRange;
}

bool Cron::checkExecute(unsigned short minute, unsigned short hour, unsigned short day, unsigned short month, unsigned short week)
{
  unsigned long tmp = 1;
  return (minuteRange & (tmp << minute)) > 0
    && (hourRange & (tmp << hour)) > 0
    && (dayRange & (tmp << day)) > 0
    && (monthRange & (tmp << month)) > 0
    && (weekRange & (tmp << week)) > 0;
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
  
  if (itemList.size() != 5) throw std::invalid_argument("The Cron format is incorrect: " + cronBak);

  return Cron(
    parseItem(itemList[0], 0, 59),
    parseItem(itemList[1], 0, 23),
    parseItem(itemList[2], 1, 31),
    parseItem(itemList[3], 1, 12),
    parseItem(itemList[4], 0, 6)
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
