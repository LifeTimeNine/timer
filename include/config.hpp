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
};