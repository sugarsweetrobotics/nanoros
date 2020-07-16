#pragma once

#include <vector>
#include <string>
#include <optional>
#include <memory>

#define NANOROS_API



#define ROS_MASTER_DEFAULT_PORT 11311

namespace ssr {

  namespace nanoros {


    class NANOROS_API ROSMaster {


    public:
      ROSMaster() {};
      virtual ~ROSMaster() {};

    public:

      virtual std::optional<std::vector<std::string>> getTopicNameList() = 0;
      

      
    };
    
    NANOROS_API std::shared_ptr<ROSMaster> rosmaster(const std::string& addr, const int32_t port);

  }
}
