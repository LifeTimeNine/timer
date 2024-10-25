#include "notify.hpp"
#include "ada.h"
#include "log.hpp"

namespace notify
{
  httplib::Result request(const std::string url, const nlohmann::json body)
  {
    ada::result<ada::url_aggregator> urlResult = ada::parse(url);
    httplib::Client client(urlResult->get_origin());
    return client.Post(
      std::string(urlResult->get_pathname()) + std::string(urlResult->get_search()) + std::string(urlResult->get_hash()),
      body.dump(),
      "application/json"
    );
  }

  bool taskStart(const std::string url, const message::RunBeforeNotify* runBeforeNotify)
  {
    if (url.empty()) return true;
    Log::info("<{}> task_start [uuid:{},start_time:{},next_run_time:{}]",
      "notify",
      runBeforeNotify->uuid,
      runBeforeNotify->startTime,
      runBeforeNotify->nextRunTime);
    nlohmann::json json;
    json["event"] = message::NotifyEvent::TaskStart;
    json["data"] = *runBeforeNotify;
    httplib::Result result = request(url, json);
    return result.error() == httplib::Error::Success && result.value().status == 200;
  }

  bool taskFinish(const std::string url, const message::RunResult* runResult)
  {
    nlohmann::json json;
    json["event"] = message::NotifyEvent::TaskFinish;
    json["data"] = *runResult;
    Log::info("<{}> task_finish [uuid:{},start_time:{},runtime:{},is_normal_exit:{},out:{},err:{}]",
      "notify",
      runResult->uuid,
      runResult->startTime,
      runResult->runtime,
      runResult->isNormalExit,
      runResult->out,
      runResult->err);
    httplib::Result res = request(url, json);
    return res.error() == httplib::Error::Success && res.value().status == 200;
  }
}