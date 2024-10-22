#pragma once

#include "httplib.h"
#include "nlohmann/json.hpp"
#include "message.hpp"

namespace notify
{
  /**
   * 请求
   * @param url     请求地址
   * @param body    请求内容
   */
  httplib::Result request(const std::string url, const nlohmann::json body);

  /**
   * 任务开始
   * @param url   通知地址
   * @param uuid  任务ID
   */
  bool taskStart(const std::string url, const std::string uuid);

  /**
   * 任务结束
   * @param url     通知地址
   * @param result  运行结果
   */
  bool taskFinish(const std::string url, const message::Result* result);
}