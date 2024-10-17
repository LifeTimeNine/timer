#include <iostream>
#include <string>
#include <map>
#include "taskTable.hpp"
#include "log.hpp"
#include "cron.hpp"


TaskTable::TaskTable(std::string dbPath): dbPath(dbPath), db(nullptr), table(), mutex()
{
  std::lock_guard<std::mutex> lock(mutex);
  table.clear();
  // 打开数据库
  if (sqlite3_open(dbPath.c_str(), &db) != SQLITE_OK) {
    Log::error("db open fail: " + std::string(sqlite3_errmsg(db)), "task_table");
    return;
  }
  Log::trace("db open success", "task_table");
  char* errmsg;
  int result;
  // 创建Table
  std::string createTableSql("CREATE TABLE IF NOT EXISTS task (uuid TEXT PRIMARY KEY, title TEXT, exec_file TEXT, args TEXT, loop INTEGER, enable INTEGER, cron TEXT);");
  result = sqlite3_exec(db, createTableSql.c_str(), nullptr, nullptr, &errmsg);
  if (result != SQLITE_OK) {
    Log::error("sqlite table create fail: " + std::string(errmsg), "task_table");
    sqlite3_free(errmsg);
    return;
  }
  sqlite3_free(errmsg);
  Log::info("table initiate success", "task_table");
  // 查询所有存储的任务数据
  std::string selectTaskSql("SELECT * FROM task;");
  sqlite3_stmt* stmt;
  if (sqlite3_prepare_v2(db, selectTaskSql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
    std::cout << "2" << std::endl;
    Log::error("sqlite select task fail: " + std::string(sqlite3_errmsg(db)), "task_table");
    return;
  }
  
  while (sqlite3_step(stmt) == SQLITE_ROW)
  {
    Task task;
    task.uuid = std::string((char *)sqlite3_column_text(stmt, 0));
    task.appMark = std::string((char *)sqlite3_column_text(stmt, 1));
    task.appName = std::string((char *)sqlite3_column_text(stmt, 2));
    task.title = std::string((char *)sqlite3_column_text(stmt, 3));
    task.execFile = std::string((char *)sqlite3_column_text(stmt, 4));
    task.args = std::string((char *)sqlite3_column_text(stmt, 5));
    task.loop = sqlite3_column_int64(stmt, 6) == 1;
    task.enable = sqlite3_column_int64(stmt, 7) == 1;
    task.cron = std::string((char *)sqlite3_column_text(stmt, 8));

    if (task.loop) {
      try
      {
        task.cronRange = Cron::parse(task.cron);
      }
      catch(const std::exception& e)
      {
        Log::error("Cron parse fail: " + task.cron, "task_table");
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

bool TaskTable::initiate()
{
  
  return true;
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
      Log::error("Cron parse fail: " + task.cron, "task_table");
    }
  }

  table[task.uuid] = task;
  sqlite3_stmt* stmt;
  std::string sql;
  if (isExist) {
    sql = "UPDATE task SET app_mark=?,app_name=?,title=?,exec_file=?,args=?,loop=?,enable=?,cron=? WHERE uuid=?;";
  } else {
    sql = "INSERT INTO task (app_mark,app_name,title,exec_file,args,loop,enable,cron,uuid) VALUES(?,?,?,?,?,?,?,?,?);";
  }
  if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
    Log::error("sqlite " + std::string(isExist?"update":"insert") + " sql prepare fail: [" + task.uuid + "] " + std::string(sqlite3_errmsg(db)), "task_table");
    return;
  }
  sqlite3_bind_text(stmt, 1, task.appMark.c_str(), -1, SQLITE_STATIC);
  sqlite3_bind_text(stmt, 2, task.appName.c_str(), -1, SQLITE_STATIC);
  sqlite3_bind_text(stmt, 3, task.title.c_str(), -1, SQLITE_STATIC);
  sqlite3_bind_text(stmt, 4, task.execFile.c_str(), -1, SQLITE_STATIC);
  sqlite3_bind_text(stmt, 5, task.args.c_str(), -1, SQLITE_STATIC);
  sqlite3_bind_int(stmt, 6, task.loop ? 1 : 0);
  sqlite3_bind_int(stmt, 7, task.enable ? 1 : 0);
  sqlite3_bind_text(stmt, 8, task.cron.c_str(), -1, SQLITE_STATIC);
  sqlite3_bind_text(stmt, 9, task.uuid.c_str(), -1, SQLITE_STATIC);
  if (sqlite3_step(stmt) != SQLITE_DONE) {
    Log::error("sqlite " + std::string(isExist?"update":"insert") + " fail: [" + task.uuid + "] " + std::string(sqlite3_errmsg(db)), "task_table");
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
  std::string sql("DELETE FROM task WHERE uuid=?");
  if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
    Log::error("sqlite delete sql prepare fail: [" + uuid + "] " + std::string(sqlite3_errmsg(db)), "task_table");
    return;
  }
  sqlite3_bind_text(stmt, 1, uuid.c_str(), -1, SQLITE_STATIC);
  if (sqlite3_step(stmt) != SQLITE_DONE) {
    Log::error("sqlite delete fail: [" + uuid + "] " + std::string(sqlite3_errmsg(db)), "task_table");
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
