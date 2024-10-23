#pragma once

#include <iostream>
#include "config.hpp"
#include "taskTable.hpp"
#include "httplib.h"
#include "nlohmann/json.hpp"

/**
 * Http响应状态码
 */
enum Status
{
  /** 正常 */
  Normal,
  /** 任务不存在 */
  TaskNotExit = 1001,
  /** 参数解析失败 */
  ParamsParseFail = 1002
};

/**
 * Http响应
 */
template <typename T>
struct Response
{
  Status status;
  T* data;
  std::string message;

  friend void to_json(nlohmann::json &json, struct Response response)
  {
    json["status"] = static_cast<unsigned int>(response.status);
    if (response.data == nullptr) {
      json["data"] = nullptr;
    } else {
      json["data"] = *(response.data);
    }
    json["message"] = response.message;
  };
};

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
    void response(httplib::Response& response, Status status, T* data, std::string message = "success");
};
