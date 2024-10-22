#include "process.hpp"
#include <unistd.h>
#include <sys/wait.h>
#include <sstream>

Process::Data::Data(): pid(-1) {}

Process::Process(const std::string &path, const std::string &args,
          size_t bufferSize):
          bufferSize(bufferSize), stdoutContent(), stderrContent(), data() {
  std::istringstream iss(args);
  std::vector<char*> argv;
  std::string arg;
  while(iss >> arg) argv.push_back(arg.data());
  open([this, &path, &argv]() {
    auto res = execv(path.data(), argv.data());
    std::cout << "exec: " << res << std::endl;
  });
  asyncRead();
}

pid_t Process::getPid() const {
  return data.pid;
}

void Process::open(std::function<void()> function) {
  stdinFd = std::unique_ptr<int>(new int);
  stdoutFd = std::unique_ptr<int>(new int);
  stderrFd = std::unique_ptr<int>(new int);

  int stdinP[2], stdoutP[2], stderrP[2];

  if(stdinFd && pipe(stdinP)!=0)
    return;
  if(stdoutFd && pipe(stdoutP)!=0) {
    if(stdinFd) {close(stdinP[0]);close(stdinP[1]);}
    return;
  }
  if(stderrFd && pipe(stderrP)!=0) {
    if(stdinFd) {close(stdinP[0]);close(stdinP[1]);}
    if(stdoutFd) {close(stdoutP[0]);close(stdoutP[1]);}
    return;
  }
  
  pid_t pid = fork();
  if (pid < 0) {
    if(stdinFd) {close(stdinP[0]);close(stdinP[1]);}
    if(stdoutFd) {close(stdoutP[0]);close(stdoutP[1]);}
    if(stderrFd) {close(stderrP[0]);close(stderrP[1]);}
    return;
  }
  else if (pid > 0) {
    if(stdinFd) close(stdinP[0]);
    if(stdoutFd) close(stdoutP[1]);
    if(stderrFd) close(stderrP[1]);
    
    if(stdinFd) *stdinFd = stdinP[1];
    if(stdoutFd) *stdoutFd = stdoutP[0];
    if(stderrFd) *stderrFd = stderrP[0];

    data.pid=pid;
    return;
  }
  else if (pid == 0) {
    if(stdinFd) {dup2(stdinP[0], 0);close(stdinP[1]);}
    if(stdoutFd) {dup2(stdoutP[1], 1);close(stdoutP[0]);}
    if(stderrFd) {dup2(stderrP[1], 2);close(stderrP[0]);}

    int fd_max=static_cast<int>(sysconf(_SC_OPEN_MAX));
    for(int fd=3;fd<fd_max;fd++)
      close(fd);
  
    setpgid(0, 0);

    function();
    
    exit(EXIT_FAILURE);
  }
}

void Process::asyncRead() {
  if(data.pid<=0)
    return;
  if(stdoutFd) {
    stdoutThread=std::thread([this](){
      auto buffer = std::unique_ptr<char[]>( new char[bufferSize] );
      ssize_t n;
      while ((n=read(*stdoutFd, buffer.get(), bufferSize)) > 0)
        stdoutContent += std::string(buffer.get(), static_cast<size_t>(n));
    });
  }
  if(stderrFd) {
    stderrThread=std::thread([this](){
      auto buffer = std::unique_ptr<char[]>( new char[bufferSize] );
      ssize_t n;
      while ((n=read(*stderrFd, buffer.get(), bufferSize)) > 0)
        stderrContent += std::string(buffer.get(), static_cast<size_t>(n));
    });
  }
}

int Process::getExitStatus() {
  if(data.pid<=0)
    return -1;
  int exitStatus;
  waitpid(data.pid, &exitStatus, 0);
  {
    std::lock_guard<std::mutex> lock(closeMutex);
  }
  closeFds();

  if(exitStatus>=256)
    exitStatus >>= 8;
  return exitStatus;
}

void Process::closeFds() {
  if(stdoutThread.joinable())
    stdoutThread.join();
  if(stderrThread.joinable())
    stderrThread.join();
  
  if(stdinFd)
    closeStdin();
  if(stdoutFd) {
    if(data.pid > 0)
      close(*stdoutFd);
    stdoutFd.reset();
  }
  if(stderrFd) {
    if(data.pid > 0)
      close(*stderrFd);
    stderrFd.reset();
  }
}

bool Process::write(const std::string &data) {
  std::lock_guard<std::mutex> lock(stdinMutex);
  if(stdinFd) {
    if(::write(*stdinFd, data.data(), data.size()) >= 0) {
      return true;
    }
    else {
      return false;
    }
  }
  return false;
}

void Process::closeStdin() {
  std::lock_guard<std::mutex> lock(stdinMutex);
  if(stdinFd) {
    if(data.pid>0)
      close(*stdinFd);
    stdinFd.reset();
  }
}

void Process::kill(unsigned short signal) {
  std::lock_guard<std::mutex> lock(closeMutex);
  if(data.pid > 0) {
    ::kill(-data.pid, signal);
  }
}

std::string Process::getStdout() const {
  return stdoutContent;
}

std::string Process::getStderr() const {
  return stderrContent;
}

void kill(pid_t pid, unsigned short signal) {
  if (pid <= 0) return;
  ::kill(-pid, signal);
}
