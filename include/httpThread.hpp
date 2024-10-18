#pragma once

#include <iostream>
#include "config.hpp"
#include "taskTable.hpp"
#include "httplib.h"

/**
 * HTTP服务
 */
class HttpThread
{
  public:
  private:
    Config* config;
    TaskTable* taskTable;
    httplib::Server server;
  public:
    /**
     * HTTP服务
     */
    HttpThread(Config* config, TaskTable* taskTable);
    /**
     * 监听
     */
    void listen();
    /**
     * 是否在运行
     */
    bool isRunning();
    /**
     * 关闭服务
     */
    void close();
    /**
     * 监控服务
     */
    void watch();
  private:
    void task();
    void running();
};
