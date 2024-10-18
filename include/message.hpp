#pragma once

#include <iostream>

#include "nlohmann/json.hpp"

namespace message
{
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
}