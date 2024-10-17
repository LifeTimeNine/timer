#pragma once

#include <iostream>

/**
 * 配置类
 */
class Config
{
  private:
    /**
     * HTTP服务地址
     */
    std::string httpHost = "127.0.0.1";
    /**
     * HTTP服务端口
     */
    unsigned short httpPort = 10010;
    /**
     * 日志目录
     */
    std::string logDir = "/var/log/timer";
    /**
     * 日志等级
     */
    unsigned short logLevel = 0;
    /**
     * 数据库路径
     */
    std::string dbPath = "/var/lib/timer/task.db";

  public:
    /** 
     * 配置类
     * @param file  配置文件地址
    */
    Config(std::string file);

    /**
     * 设置HTTP服务地址
     * @param host  地址
     */
    Config setHttpHost(std::string host);
    /**
     * 获取HTTP服务地址
     */
    std::string getHttpHost();
    /**
     * 设置HTTP服务端口
     * @param port
     */
    Config setHttpPort(unsigned short port);
    /**
     * 获取HTTP服务端口
     */
    unsigned short getHttpPort();
    /**
     * 获取日志路径
     */
    std::string getLogDir();
    /**
     * 获取日志等级
     */
    unsigned short getLogLevel();
    /**
     * 获取数据库文件路径
     */
    std::string getDbPath();
};