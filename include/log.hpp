#pragma once

#include "config.hpp"

#include "spdlog/logger.h"

/**
 * 日志
 */
class Log
{
  private:
    static spdlog::logger* logger;

  public:
    /**
     * 初始化
     * @param config  配置类
     */
    void static initiate(Config* config);
    /**
     * 清除
     */
    void static clear();

    /**
     * 
     */
};