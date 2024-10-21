#pragma once

#include <iostream>
#include <functional>
#include <mutex>
#include <thread>

/**
 * 进程管理类
 */
class Process
{
  public:
    Process(const std::string &path, const std::string &args= std::string(),
          size_t bufferSize=131072);
    ~Process() {
      closeFds();
    }
    /**
     * 获取进程PID
     */
    pid_t getPid() const;
    /**
     * 等待进程结束并获取退出状态码
     */
    int getExitStatus();
    /**
     * 写入标准输入
     */
    bool write(const std::string &data);
    /**
     * 关闭标准输入
     */
    void closeStdin();
    /**
     * 发送信号
     * @param signal  信号值
     */
    void kill(unsigned short signal);
    /**
     * 获取标准输出
     */
    std::string getStdout() const;
    /**
     * 获取异常输出
     */
    std::string getStderr() const;

    /**
     * 发送信号
     * @param pid     进程ID
     * @param signal  信号值
     */
    static void kill(pid_t pid, unsigned short signal);

  private:
    class Data
    {
      public:
        Data();
        pid_t pid;
    };
    Data data;
    std::mutex closeMutex, stdinMutex;
    std::thread stdoutThread, stderrThread;
    size_t bufferSize;
    std::unique_ptr<pid_t> stdoutFd, stderrFd, stdinFd;
    std::string stdoutContent, stderrContent;

    pid_t open(std::function<void()> function);
    void asyncRead();
    void closeFds();
};