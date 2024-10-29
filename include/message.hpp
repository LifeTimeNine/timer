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
   * 状态
   */
  class State
  {
    public:
      size_t taskTotal;
      size_t runningNumber;

    friend void to_json(nlohmann::json& json, const State& state)
    {
      json["task_total"] = state.taskTotal;
      json["running_number"] = state.runningNumber;
    }
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

  namespace notify
  {
    /**
     * 运行前通知
     */
    class RunBefore
    {
      public:
        /* 任务UUID */
        std::string uuid;
        /** 开始时间 */
        std::string startTime;
        /** 下一次运行时间 */
        std::string nextRunTime;

      friend void to_json(nlohmann::json& json, const RunBefore& runBefore)
      {
        json["uuid"] = runBefore.uuid;
        json["start_time"] = runBefore.startTime;
        json["next_run_time"] = runBefore.nextRunTime;
      }
    };

    /**
     * 运行结果
     */
    class RunResult
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
      
      friend void to_json(nlohmann::json& json, const RunResult& runResult)
      {
        json["uuid"] = runResult.uuid;
        json["start_time"] = runResult.startTime;
        json["end_time"] = runResult.endTime;
        json["runtime"] = runResult.runtime;
        json["is_normal_exit"] = runResult.isNormalExit;
        json["out"] = runResult.out;
        json["err"] = runResult.err;
      }
    };
  }

  namespace http
  {
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
     * 保存任务信息
     */
    class TaskSave
    {
      public:
      std::string execFile;
      std::string args;
      bool loop = false;
      bool enable = false;
      std::string cron;

      friend void to_json(nlohmann::json& json, const TaskSave& task)
      {
        json["exec_file"] = task.execFile;
        json["args"] = task.args;
        json["loop"] = task.loop;
        json["enable"] = task.enable;
        json["cron"] = task.cron;
      }

      friend void from_json(const nlohmann::json& json, TaskSave& task)
      {
        task.execFile = json.at("exec_file");
        task.args = json.at("args");
        task.loop = json.at("loop");
        task.enable = json.at("enable");
        task.cron = json.at("cron");
      }
    };

    /**
     * 任务状态信息
     */
    class TaskStatus
    {
      public:
      bool enable = false;

      friend void to_json(nlohmann::json& json, const TaskStatus& taskStatus)
      {
        json["enable"] = taskStatus.enable;
      }

      friend void from_json(const nlohmann::json& json, TaskStatus& taskStatus)
      {
        taskStatus.enable = json.at("enable");
      }
    };

    /**
     * Http响应
     */
    template <typename T>
    struct Response
    {
      ResponseStatus status;
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
  }
}
