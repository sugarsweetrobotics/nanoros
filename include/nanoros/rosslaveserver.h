#pragma once

#include <string>
#include <memory>

#include "nanoros/rosmaster.h"

namespace ssr {
  namespace nanoros {
    class ROSNode;

    class ROSSlaveServer {
    public:
      ROSSlaveServer() {}
      virtual ~ROSSlaveServer() {}

    public:
      virtual std::string getSlaveUri() const = 0;
      virtual std::string getSlaveIP() const = 0;
      virtual bool standByNode(const std::string& topicName, const std::string& caller_id, const std::string& selfIP, const int32_t port) { return false; }

    };

    std::shared_ptr<ROSSlaveServer> rosslaveserver(ROSNode* rosnode, const std::shared_ptr<ROSMaster>& master, const std::string& selfIp, const int32_t port);
  }
}
