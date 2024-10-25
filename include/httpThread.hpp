#pragma once

#include <iostream>
#include "config.hpp"
#include "taskTable.hpp"
#include "httplib.h"
#include "nlohmann/json.hpp"
#include "message.hpp"

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
     * 启动
     */
    void start();
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
    /**
     * 注册任务相关接口
     */
    void task();
    /**
     * 注册运行接口
     */
    void running();
    /**
     * 响应数据
     * @param response  响应类
     * @param status    状态码
     * @param data      数据
     * @param message   消息
     */
    template <typename T>
    void response(httplib::Response& response, message::ResponseStatus status, T* data, std::string message = "success");
};
