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

  bool taskStart(const std::string url, const std::string uuid)
  {
    if (url.empty()) return true;
    Log::info("<{}> task_start [uuid: {}]", "notify", uuid);
    message::TaskOperation taskOperation;
    taskOperation.uuid = uuid;
    nlohmann::json json;
    json["event"] = message::NotifyEvent::TaskStart;
    json["data"] = taskOperation;
    httplib::Result result = request(url, json);
    return result.error() == httplib::Error::Success && result.value().status == 200;
  }

  bool taskFinish(const std::string url, const message::Result* result)
  {
    nlohmann::json json;
    json["event"] = message::NotifyEvent::TaskFinish;
    json["data"] = *result;
    Log::info("<{}> task_finish [{}]", "notify", json["data"].dump());
    httplib::Result res = request(url, json);
    return res.error() == httplib::Error::Success && res.value().status == 200;
  }
}