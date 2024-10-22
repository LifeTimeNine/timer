#include "httpThread.hpp"
#include "message.hpp"
#include "log.hpp"
#include "util.hpp"
#include "nlohmann/json.hpp"
#include <vector>
#include <thread>


HttpThread::HttpThread(Config* config, TaskTable* taskTable):
config(config),
taskTable(taskTable),
server()
{
  server.Get("/", [this](const httplib::Request& request, httplib::Response& response) {
    response.set_content("Timer", "text/plain");
  });
  task();
  running();
}

void HttpThread::start()
{
  std::thread t([this]() {
    Log::info("<{}> listen: {}:{}", "http_server", config->getHttpHost(), config->getHttpPort());
    server.listen(config->getHttpHost(), config->getHttpPort());
  });
  t.detach();
}

bool HttpThread::isRunning()
{
  return server.is_running();
}

void HttpThread::close()
{
  server.stop();
}

void HttpThread::watch()
{
  if (!isRunning()) {
    start();
  }
}

void HttpThread::task()
{
  server.Get("/task", [this](const httplib::Request& request, httplib::Response& response) {
    Log::info("<{}> get task", "http_server");
    if (request.has_param("uuid")) {
      if (taskTable->exist(request.get_param_value("uuid"))) {
        Task taskInfo = taskTable->get(request.get_param_value("uuid"));
        message::Task task;
        task.uuid = taskInfo.uuid;
        task.execFile = taskInfo.execFile;
        task.args = taskInfo.args;
        task.cron = taskInfo.cron;
        task.loop = taskInfo.loop;
        task.enable = taskInfo.enable;
        task.cron = taskInfo.cron;
        nlohmann::json json = task;
        response.set_content(json.dump(), "application/json");
      } else {
        response.status = httplib::StatusCode::NotFound_404;
        response.set_content("Task does not exist", "text/plain");
      }
    } else {
      std::vector<message::Task> result;
      for(Task item: taskTable->list()) {
        message::Task task;
        task.uuid = item.uuid;
        task.execFile = item.execFile;
        task.args = item.args;
        task.cron = item.cron;
        task.loop = item.loop;
        task.enable = item.enable;
        task.cron = item.cron;
        result.push_back(task);
      }
      nlohmann::json json = result;
      response.set_content(json.dump(), "application/json");
    }
  });

  server.Post("/task", [this](const httplib::Request& request, httplib::Response& response) {
    Log::info("<{}> post task", "http_server");
    message::Task task;
    try
    {
      nlohmann::json::parse(request.body).get_to(task);
    }
    catch(const std::exception& e)
    {
      Log::warn("<{}> post task  json parse fail: {}", "http_server", request.body);
      response.status = httplib::StatusCode::NotImplemented_501;
      response.set_content("fail", "text/plain");
      return;
    }
    // 保存信息
    Task saveTask;
    saveTask.uuid = task.uuid;
    saveTask.execFile = task.execFile;
    saveTask.args = task.args;
    saveTask.loop = task.loop;
    saveTask.enable = task.enable;
    saveTask.cron = task.cron;
    taskTable->set(saveTask);
    response.set_content("success", "text/plain");
  });

  server.Delete("/task", [this](const httplib::Request& request, httplib::Response& response) {
    Log::info("<{}> delete task", "http_server");

    message::TaskOperation taskOperation;
    try
    {
      nlohmann::json::parse(request.body).get_to(taskOperation);
    }
    catch(const std::exception& e)
    {
      Log::warn("<{}> delete task  json parse fail: {}", "http_server", request.body);
      response.set_content("fail", "text/plain");
      return;
    }

    // 获取任务信息
    if (!taskTable->exist(taskOperation.uuid)) {
      response.status = httplib::StatusCode::NotFound_404;
      response.set_content("Task does not exist", "text/plain");
      return;
    }
    // 从任务表删除
    taskTable->remove(taskOperation.uuid);
    response.set_content("success", "text/plain");
  });
}

void HttpThread::running()
{
  server.Post("/run", [this](const httplib::Request& request, httplib::Response& response) {
    Log::info("<{}> post run", "http_server");
    message::TaskOperation taskOperation;
    try
    {
      nlohmann::json::parse(request.body).get_to(taskOperation);
    }
    catch(const std::exception& e)
    {
      Log::warn("<{}> delete task  json parse fail: {}", "http_server", request.body);
      response.set_content("fail", "text/plain");
      return;
    }
    // 获取任务信息
    if (!taskTable->exist(taskOperation.uuid)) {
      response.status = httplib::StatusCode::NotFound_404;
      response.set_content("Task does not exist", "text/plain");
      return;
    }
    Task task = taskTable->get(taskOperation.uuid);
    // 运行任务
    task.run(config);
    response.set_content("success", "text/plain");
  });
}
