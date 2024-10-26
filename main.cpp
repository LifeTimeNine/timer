#include <iostream>
#include <csignal>
#include <sys/time.h>
#include "cmdline.h"
#include "config.hpp"
#include "log.hpp"
#include "taskTable.hpp"
#include "httpThread.hpp"
#include "util.hpp"

Config* config = nullptr;
TaskTable* taskTable = nullptr;
HttpThread* httpThread = nullptr;

static cmdline::parser arg;
static bool isRunning = true;
static bool isReload = false;

int main(int argc, char *argv[])
{
  // 解析参数
  arg.add<std::string>("config", 'c', "Config File", false, "/etc/timer.ini");
  arg.add<std::string>("host", 'h', "HTTP Host", false);
  arg.add<unsigned short>("port", 'p', "HTTP Port", false);
  arg.parse_check(argc, argv);

  // 初始化配置类
  config = new Config(arg.get<std::string>("config"));
  if (arg.exist("host")) config->setHttpHost(arg.get<std::string>("host"));
  if (arg.exist("port")) config->setHttpPort(arg.get<unsigned short>("port"));

  // 获取PID文件中的数据
  std::string pidFileData = util::fileGetContents(config->getPidFile());
  if (!pidFileData.empty() && kill(std::stoi(pidFileData), 0) == 0) {
    std::cerr << "Running!" << std::endl;
    return EXIT_FAILURE;
  }

  // 初始化日志模块
  Log::initiate(config->getLogDir(), config->getLogLevel());

  if (!util::filePutContents(config->getPidFile(), std::to_string(getpid()))) {
    Log::error("Pid file write fail! [{}]", config->getPidFile());
    return EXIT_FAILURE;
  }

  try
  {
    taskTable = new TaskTable(config->getDbPath());
  }
  catch(const std::exception& e)
  {
    Log::error("Task Table initiate fail: {}", e.what());
    return EXIT_FAILURE;
  }

  // 初始化HTTP线程
  httpThread = new HttpThread(config, taskTable);
  httpThread->start();

  // 监听定时器信号
  signal(SIGALRM, [](int signal) {
    // 如果正在重载，本次不处理
    if (isReload) return;
    tm tm = util::getLocaltime();

    // 循环任务表
    for(Task task : taskTable->list()) {
      // 如果不是循环执行，或者未启用
      if (!task.loop || !task.enable) continue;
      if (task.cronRange.checkRunTime(tm.tm_sec, tm.tm_min, tm.tm_hour, tm.tm_mday, tm.tm_mon + 1, tm.tm_wday)) {
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

  // 监听退出信号
  signal(SIGTERM, [](int signal) {
    // 退出HTTP线程
    httpThread->close();
    // 清除定时器
    setitimer(ITIMER_REAL, nullptr, nullptr);

    isRunning = false;
  });

  // 监听重载信号
  signal(SIGHUP, [](int signal) {
    // 标记为正在重载
    isReload = true;
    Log::info("Reload!");
    // 退出HTTP线程
    httpThread->close();
    // 清除任务表
    delete taskTable;
    // 清除配置类
    delete config;

    // 初始化配置类
    config = new Config(arg.get<std::string>("config"));
    if (arg.exist("host")) config->setHttpHost(arg.get<std::string>("host"));
    if (arg.exist("port")) config->setHttpPort(arg.get<unsigned short>("port"));
    // 初始化日志模块
    Log::initiate(config->getLogDir(), config->getLogLevel());
    if (!util::filePutContents(config->getPidFile(), std::to_string(getpid()))) {
      Log::error("Pid file write fail! [{}]", config->getPidFile());
      exit(EXIT_FAILURE);
    }
    // 初始化任务表
    try
    {
      taskTable = new TaskTable(config->getDbPath());
    }
    catch(const std::exception& e)
    {
      Log::error("Task Table initiate fail: {}", e.what());
      exit(EXIT_FAILURE);
    }
    // 初始化HTTP线程
    httpThread = new HttpThread(config, taskTable);
    httpThread->start();
    isReload = false;
  });

  while (isRunning)
  {
    pause();
  }

  std::filesystem::remove(config->getPidFile());

  delete httpThread;
  delete taskTable;
  delete config;

  return EXIT_SUCCESS;
}