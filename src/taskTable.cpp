#include <iostream>
#include <string>
#include <map>
#include <sys/stat.h>
#include <thread>
#include "taskTable.hpp"
#include "log.hpp"
#include "cron.hpp"
#include "process.hpp"
#include "message.hpp"
#include "util.hpp"
#include "notify.hpp"

TaskTable::TaskTable(std::string dbPath): dbPath(dbPath), db(nullptr), table(), mutex(), runCountAtomic(0)
{
  std::lock_guard<std::mutex> lock(mutex);
  table.clear();
  size_t found = dbPath.find_last_of("/");
  std::string dir = dbPath.substr(0, found);
  struct stat info;
  if (stat(dir.data(), &info) != 0 || (info.st_mode & S_IFDIR) == 0 ) {
    Log::info("<{}> DB dir not exits: {}", "task_table", dir);
    // 创建目录
    std::string command = "mkdir -p " + dir;
    if (system(command.data()) != 0) {
      throw std::runtime_error("db dir create fail[" + dir + "]");
    }
  }
  // 打开数据库
  if (sqlite3_open(dbPath.c_str(), &db) != SQLITE_OK) {
    throw std::runtime_error("db open fail[" + std::string(sqlite3_errmsg(db)) + "]");
  }
  // 连接密码
  std::string password = "timer_1411";
  // if ()
  Log::info("<{}> db path: {}", "task_table", dbPath);
  char* errmsg;
  int result;
  // 创建Table
  std::string createTableSql("CREATE TABLE IF NOT EXISTS task (uuid TEXT PRIMARY KEY, exec_file TEXT, args TEXT, loop INTEGER, enable INTEGER, cron TEXT);");
  result = sqlite3_exec(db, createTableSql.c_str(), nullptr, nullptr, &errmsg);
  if (result != SQLITE_OK) {
    throw std::runtime_error("sqlite table create fail [" + std::string(errmsg) + "]");
    sqlite3_free(errmsg);
    return;
  }
  sqlite3_free(errmsg);
  // 查询所有存储的任务数据
  std::string selectTaskSql("SELECT * FROM task;");
  sqlite3_stmt* stmt;
  if (sqlite3_prepare_v2(db, selectTaskSql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
    throw std::runtime_error("sqlite select task fail[" + std::string(sqlite3_errmsg(db)) + "]");
    return;
  }
  
  while (sqlite3_step(stmt) == SQLITE_ROW)
  {
    Task task;
    task.uuid = std::string((char *)sqlite3_column_text(stmt, 0));
    task.execFile = std::string((char *)sqlite3_column_text(stmt, 1));
    task.args = std::string((char *)sqlite3_column_text(stmt, 2));
    task.loop = sqlite3_column_int64(stmt, 3) == 1;
    task.enable = sqlite3_column_int64(stmt, 4) == 1;
    task.cron = std::string((char *)sqlite3_column_text(stmt, 5));

    if (task.loop) {
      try
      {
        task.cronRange = Cron::parse(task.cron);
      }
      catch(const std::exception& e)
      {
        Log::error("<{}> Cron parse fail: {}", "task_table", task.cron);
      }
    }
    table[task.uuid] = task;
  }
  sqlite3_finalize(stmt);
}

TaskTable::~TaskTable()
{
  if (db != nullptr) {
    sqlite3_close(db);
    db = nullptr;
  }
}

void TaskTable::run(Task task, Config* config)
{
  std::thread t([task, config, this]() {
    runCountAtomic.fetch_add(1, std::memory_order_seq_cst);
    if (config->getNotifyUrl().empty()) Log::info("task run [uuid: {}]", task.uuid);
    // 记录开始时间
    auto startTime = std::chrono::system_clock::now();
    auto startTimePoint = date::floor<std::chrono::seconds>(startTime);
    message::notify::RunResult result;
    result.uuid = task.uuid;
    result.startTime = util::getFormatTime("%Y-%m-%d %H:%M:%S", &startTimePoint);
    // 通知任务开始运行
    std::thread taskStartNotifyThread([&task, &startTimePoint, &result, config]() {
      message::notify::RunBefore runBefore;
      runBefore.uuid = task.uuid;
      runBefore.startTime = result.startTime;
      Cron cron = task.cronRange;
      date::sys_seconds nextRunTime = cron.getNextRunTime(startTimePoint);
      runBefore.nextRunTime = util::getFormatTime("%Y-%m-%d %H:%M:%S", &nextRunTime);
      notify::taskStart(config->getNotifyUrl(), &runBefore);
    });
    taskStartNotifyThread.detach();

    Process process(task.execFile, task.args);
    result.isNormalExit = process.getExitStatus() == 0;
    auto endTime = std::chrono::system_clock::now();
    auto endTimePoint = date::floor<std::chrono::seconds>(endTime);
    result.endTime = util::getFormatTime("%Y-%m-%d %H:%M:%S", &endTimePoint);
    std::stringstream str;
    str << std::setprecision(3) << static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count()) / 1000;
    result.runtime = str.str();
    result.out = process.getStdout();
    result.err = process.getStderr();
    // 通知任务运行结束
    notify::taskFinish(config->getNotifyUrl(), &result);
    runCountAtomic.fetch_sub(1, std::memory_order_seq_cst);
  });
  t.detach();
}

bool TaskTable::exist(std::string uuid)
{
  return table.find(uuid) != table.end();
}

void TaskTable::set(Task task)
{
  std::lock_guard<std::mutex> lock(mutex);
  bool isExist = exist(task.uuid);

  if (task.loop) {
    try
    {
      task.cronRange = Cron::parse(task.cron);
    }
    catch(const std::exception& e)
    {
      Log::error("<{}> Cron parse fail: {}", "task_table", task.cron);
    }
  }

  table[task.uuid] = task;
  sqlite3_stmt* stmt;
  std::string sql;
  if (isExist) {
    sql = "UPDATE task SET exec_file=?,args=?,loop=?,enable=?,cron=? WHERE uuid=?;";
  } else {
    sql = "INSERT INTO task (exec_file,args,loop,enable,cron,uuid) VALUES(?,?,?,?,?,?);";
  }
  if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
    Log::error("<{}> sqlite {} sql prepare fail: [{}] {}", "task_table", std::string(isExist?"update":"insert"), task.uuid, std::string(sqlite3_errmsg(db)));
    return;
  }

  sqlite3_bind_text(stmt, 1, task.execFile.c_str(), -1, SQLITE_STATIC);
  sqlite3_bind_text(stmt, 2, task.args.c_str(), -1, SQLITE_STATIC);
  sqlite3_bind_int(stmt, 3, task.loop ? 1 : 0);
  sqlite3_bind_int(stmt, 4, task.enable ? 1 : 0);
  sqlite3_bind_text(stmt, 5, task.cron.c_str(), -1, SQLITE_STATIC);
  sqlite3_bind_text(stmt, 6, task.uuid.c_str(), -1, SQLITE_STATIC);

  if (sqlite3_step(stmt) != SQLITE_DONE) {
    Log::error("<{}> sqlite {} fail: [{}] {}", "task_table", std::string(isExist?"update":"insert"), task.uuid, std::string(sqlite3_errmsg(db)));
  }
  sqlite3_finalize(stmt);
}

Task TaskTable::get(std::string uuid)
{
  return table[uuid];
}

void TaskTable::remove(std::string uuid)
{
  std::lock_guard<std::mutex> lock(mutex);
  if (table.erase(uuid) == 0) return;
  sqlite3_stmt* stmt;
  std::string sql("DELETE FROM task WHERE uuid = ?");
  if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
    Log::error("<{}> sqlite delete sql prepare fail: [{}] {}", "task_table", uuid, std::string(sqlite3_errmsg(db)));
    return;
  }
  sqlite3_bind_text(stmt, 1, uuid.c_str(), -1, SQLITE_STATIC);
  if (sqlite3_step(stmt) != SQLITE_DONE) {
     Log::error("<{}> sqlite delete fail: [{}] {}", "task_table", uuid, std::string(sqlite3_errmsg(db)));
  }
  sqlite3_finalize(stmt);
}

std::vector<Task> TaskTable::list()
{
  std::vector<Task> result;
  for (std::map<std::string, Task>::iterator it = table.begin(); it != table.end(); ++it) {
    result.push_back(it->second);
  }
  return result;
}

size_t TaskTable::size()
{
  return table.size();
}

size_t TaskTable::runCount()
{
  return runCountAtomic.load(std::memory_order_seq_cst);
}
