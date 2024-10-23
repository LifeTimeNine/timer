#pragma once

#include <iostream>

#include "nlohmann/json.hpp"

namespace message
{
  /**
   * 通知事件
   */
  enum NotifyEvent
  {
    /** 任务启动 */
    TaskStart = 1,
    /** 任务完成 */
    TaskFinish
  };
  /**
   * 任务类
   */
  class Task
  {
    public:
    std::string uuid;
    std::string execFile;
    std::string args;
    bool loop = false;
    bool enable = false;
    std::string cron;

    friend void to_json(nlohmann::json& json, const Task& task)
    {
      json["uuid"] = task.uuid;
      json["exec_file"] = task.execFile;
      json["args"] = task.args;
      json["loop"] = task.loop;
      json["enable"] = task.enable;
      json["cron"] = task.cron;
    }

    friend void from_json(const nlohmann::json& json, Task& task)
    {
      task.uuid = json.at("uuid");
      task.execFile = json.at("exec_file");
      task.args = json.at("args");
      task.loop = json.at("loop");
      task.enable = json.at("enable");
      task.cron = json.at("cron");
    }
  };

  /**
   * 任务操作类
   */
  class TaskOperation
  {
    public:
      std::string uuid;

    friend void to_json(nlohmann::json& json, const TaskOperation& taskOperation)
    {
      json["uuid"] = taskOperation.uuid;
    }

    friend void from_json(const nlohmann::json& json, TaskOperation& taskOperation)
    {
      taskOperation.uuid = json.at("uuid");
    }
  };

  /**
   * 结果
   */
  class Result
  {
    public:
      /* 任务UUID */
      std::string uuid;
      /** 开始时间 */
      std::string startTime;
      /** 结束时间 */
      std::string endTime;
      /** 运行时间 */
      std::string runtime;
      /** 是否正常退出 */
      bool isNormalExit;
      /** 标准输出 */
      std::string out;
      /** 异常输出 */
      std::string err;
    
    friend void to_json(nlohmann::json& json, const Result& result)
    {
      json["uuid"] = result.uuid;
      json["start_time"] = result.startTime;
      json["end_time"] = result.endTime;
      json["runtime"] = result.runtime;
      json["is_normal_exit"] = result.isNormalExit;
      json["out"] = result.out;
      json["err"] = result.err;
    }
  };
}
