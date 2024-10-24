#include <iostream>
#include <csignal>
#include <sys/time.h>
#include "cmdline.h"
#include "config.hpp"
#include "log.hpp"
#include "taskTable.hpp"
#include "httpThread.hpp"

Config* config = nullptr;
TaskTable* taskTable = nullptr;
HttpThread* httpThread = nullptr;

static bool isRunning = true;
static bool isReload = false;

int main(int argc, char *argv[])
{
  // 解析参数
  cmdline::parser arg;
  arg.add<std::string>("config", 'c', "Config File", false, "/etc/timer.ini");
  arg.add<std::string>("host", 'h', "HTTP Host", false);
  arg.add<unsigned short>("port", 'p', "HTTP Port", false);
  arg.parse_check(argc, argv);

  // 初始化配置类
  config = new Config(arg.get<std::string>("config"));
  if (arg.exist("host")) config->setHttpHost(arg.get<std::string>("host"));
  if (arg.exist("port")) config->setHttpPort(arg.get<unsigned short>("port"));

  // 初始化日志模块
  Log::initiate(config->getLogDir(), config->getLogLevel());

  Cron cron = Cron::parse("* * * * *");

  cron.getNextRunTime(std::chrono::system_clock::now());

  return EXIT_SUCCESS;

  try
  {
    taskTable = new TaskTable(config->getDbPath());
  }
  catch(const std::exception& e)
  {
    Log::error("Task Table initiate fail: {}", e.what());
    return EXIT_FAILURE;
  }

  httpThread = new HttpThread(config, taskTable);
  httpThread->start();

  // 监听定时器信号
  signal(SIGALRM, [](int signal) {
    // 如果正在重载，本次不处理
    if (isReload) return;
    std::time_t now = time(0);
    tm* time = std::localtime(&now);

    // 循环任务表
    for(Task task : taskTable->list()) {
      // 如果不是循环执行，或者未启用
      if (!task.loop || !task.enable) continue;
      if (task.cronRange.checkExecute(time->tm_sec, time->tm_min, time->tm_hour, time->tm_mday, time->tm_mon + 1, time->tm_wday)) {
        // 运行任务
        Task::run(task, config);
      }
    }
  });

  // 设置定时器参数
  struct itimerval timer;
  timer.it_value.tv_sec = 1; // 初始定时器间隔为1秒
  timer.it_value.tv_usec = 0;
  timer.it_interval.tv_sec = 1; // 定时器间隔为1秒
  timer.it_interval.tv_usec = 0;
  setitimer(ITIMER_REAL, &timer, nullptr);

  while (isRunning)
  {
    pause();
  }

  delete config;

  return EXIT_SUCCESS;
}