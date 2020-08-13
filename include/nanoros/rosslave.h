#pragma once

#include <string>
#include <optional>
#include <memory>
#include <vector>

#include "nanoros/rosmaster.h"

namespace ssr {
  namespace nanoros {

    struct ProtocolInfo {
      const std::string name;
      const std::string arg0;
      const int32_t arg1;
      ProtocolInfo(const std::string& n, const std::string& a0, const int32_t a1):name(n), arg0(a0), arg1(a1) {}
    };

    struct RequestTopicResult: public MasterMsg {
      ProtocolInfo protocolInfo;

      RequestTopicResult(const int32_t code, const std::string& statusMessage, const ProtocolInfo& protocolInfo_) : MasterMsg(code, statusMessage), protocolInfo(protocolInfo_) {}
      virtual ~RequestTopicResult() {}
    };



    class ROSSlave {
    public:
      ROSSlave() {}
      virtual ~ROSSlave() {}

    public:
      virtual std::optional<RequestTopicResult> requestTopic(const std::string& caller_id,
							     const std::string& topicName,
							     const std::vector<ProtocolInfo>& info) { return std::nullopt; }

      virtual std::optional<MasterMsg> publisherUpdate(const std::string& caller_id,
          const std::string& topic_name,
          const std::vector<std::string>& publishers) {
          return std::nullopt;
      }
    };

    std::shared_ptr<ROSSlave> rosslave(const std::string& publisherUri);
  }
}
