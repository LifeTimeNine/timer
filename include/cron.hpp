#pragma once

#include <iostream>

/**
 * Cron 类
 */
class Cron
{
private:
  unsigned long minuteRange;
  unsigned int hourRange;
  unsigned int dayRange;
  unsigned short monthRange;
  unsigned short weekRange;
public:
  Cron(
    unsigned long minuteRange = 0,
    unsigned int hourRange = 0,
    unsigned int dayRange = 0,
    unsigned short monthRange = 0,
    unsigned short weekRange = 0
  );

  /**
   * 获取分钟范围
   */
  unsigned long getMinuteRange();
  /**
   * 获取小时范围
   */
  unsigned int getHourRange();
  /**
   * 获取天范围
   */
  unsigned int getDayRange();
  /**
   * 获取月范围
   */
  unsigned short getMonthRange();
  /**
   * 获取周范围
   */
  unsigned short getWeekRange();

  /**
   * 验证是否可以执行
   * @param minute  分钟
   * @param hour    小时
   * @param day     天
   * @param month   月
   * @param week    周
   */
  bool checkExecute(unsigned short minute, unsigned short hour, unsigned short day, unsigned short month, unsigned short week);

  /**
   * 解析
   * @param cron  Cron表达式
   */
  static Cron parse(std::string cron);

private:
  /**
   * 解析单个项
   * @param cronItem  单个项的字符串
   * @param min       最小值
   * @param max       最大值
   */
  static unsigned long parseItem(std::string cronItem, unsigned short min, unsigned short max);
};