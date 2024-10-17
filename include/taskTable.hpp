#pragma once

#include <iostream>
#include <string>
#include <map>
#include <mutex>
#include <vector>

#include "sqlite3.h"

#include "cron.hpp"

// 任务信息
class Task
{
  public:
    std::string uuid;
    std::string appMark;
    std::string appName;
    std::string title;
    std::string execFile;
    std::string args;
    bool loop;
    bool enable;
    std::string cron;

    Cron cronRange;
    unsigned int nextRunningTime = 0;

    Task(): cronRange() {}
};

/**
 * 任务表
 */
class TaskTable
{
private:
  std::string dbPath;
  sqlite3* db;
  std::map<std::string, Task> table;
  std::mutex mutex;
public:
  /** 
   * 任务表
   * @param dbPath  数据库文件路径
  */
  TaskTable(std::string dbPath);
  ~TaskTable();

  /**
   * 初始化
   */
  bool initiate();

  /**
   * 检测任务是否存在
   * @param   string  uuid UUID
   */
  bool exist(std::string uuid);

  /**
   * 设置任务
   * @param   Task    task      任务信息
   */
  void set(Task task);

  /**
   * 获取任务
   * @param   string  uuid UUID
   */
  Task get(std::string uuid);

  /**
   * 删除任务
   * @param   string  uuid UUID
   */
  void remove(std::string uuid);

  /**
   * 获取列表
   */
  std::vector<Task> list();
};
