#pragma once

#include <iostream>
#include <chrono>

/**
 * Cron 类
 */
class Cron
{
private:
  unsigned long secondRange;
  unsigned long minuteRange;
  unsigned int hourRange;
  unsigned int dayRange;
  unsigned short monthRange;
  unsigned short weekRange;
  unsigned long second;
  unsigned long minute;
  unsigned int hour;
  unsigned int day;
  unsigned short month;
  unsigned short week;
public:
  Cron(
    unsigned long second = 0,
    unsigned long minute = 0,
    unsigned int hour = 0,
    unsigned int day = 0,
    unsigned short month = 0,
    unsigned short week = 0
  );

  /**
   * 获取秒有效范围
   */
  unsigned long getSecond();
  /**
   * 获取分钟有效范围
   */
  unsigned long getMinute();
  /**
   * 获取小时有效范围
   */
  unsigned int getHour();
  /**
   * 获取天有效范围
   */
  unsigned int getDay();
  /**
   * 获取月有效范围
   */
  unsigned short getMonth();
  /**
   * 获取周有效范围
   */
  unsigned short getWeek();

  /**
   * 验证是否可以执行
   * @param minute  分钟
   * @param hour    小时
   * @param day     天
   * @param month   月
   * @param week    周
   */
  bool checkExecute(unsigned short second, unsigned short minute, unsigned short hour, unsigned short day, unsigned short month, unsigned short week);

  /**
   * 解析
   * @param cron  Cron表达式
   */
  static Cron parse(std::string cron);

  /**
   * 获取下一次执行的时间
   * @param time  当前时间
   */
  std::chrono::_V2::system_clock::time_point getNextRunTime(std::chrono::_V2::system_clock::time_point time);

private:
  /**
   * 解析单个项
   * @param cronItem  单个项的字符串
   * @param min       最小值
   * @param max       最大值
   */
  static unsigned long parseItem(std::string cronItem, unsigned short min, unsigned short max);

  /**
   * 计算下一次运行的月
   * @param time      当前时间
   * @param satisfied 当前时间是否满足
   */
  void calculateNextRunMonth(std::chrono::seconds &time, bool satisfied = true);
  /**
   * 计算下一次运行的天
   * @param time      当前时间
   * @param satisfied 当前时间是否满足
   */
  void calculateNextRunDay(std::chrono::seconds &time, bool satisfied = true);
  /**
   * 计算下一次运行的小时
   * @param time      当前时间
   * @param satisfied 当前时间是否满足
   */
  void calculateNextRunHour(std::chrono::seconds &time, bool satisfied = true);
  /**
   * 计算下一次运行的分钟
   * @param time      当前时间
   * @param satisfied 当前时间是否满足
   */
  void calculateNextRunMinute(std::chrono::seconds &time, bool satisfied = true);
  /**
   * 计算下一次运行的秒
   * @param time      当前时间
   * @param satisfied 当前时间是否满足
   */
  void calculateNextRunSecond(std::chrono::seconds &time, bool satisfied = true);
  /**
   * 计算下一次运行的周
   * @param time      当前时间
   * @param satisfied 当前时间是否满足
   */
  void calculateNextRunWeek(std::chrono::seconds &time, bool satisfied = true);

  /**
   * 获取时间结构体
   * @param time
   */
  tm* getTime(const std::chrono::seconds* time);
};