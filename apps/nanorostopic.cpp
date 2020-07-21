#include <iostream>
#include <set>
#include "nanoros/os.h"
#include "nanoros/nanoros.h"
#include "nanoros/rosutil.h"
#include "nanoros/rosmaster.h"
#include "nanoros/rosnode.h"
#include "nanoros/rosmsgstubfactory.h"

using namespace ssr::nanoros;

std::shared_ptr<ROSMaster> getROSMaster() {
  auto info = ssr::nanoros::getROSMasterInfo();
  if (!info) {
    std::cout << "Can not get ROS_MASTER_URI info." << std::endl;
    return nullptr;
  }
  auto& [addr, port] = info.value();
  return ssr::nanoros::rosmaster(addr, port);
}


std::optional<std::set<std::string>> topicNames(const std::optional<SystemState>& state) {
  if (!state) {return std::nullopt;}
  std::set<std::string> topicNameSet;
  for(auto& tpi : state->systemState.publishers) {
    topicNameSet.insert(tpi.topicName);
  }
  for(auto& tsi : state->systemState.subscribers) {
    topicNameSet.insert(tsi.topicName);
  }
  return topicNameSet;
}

template<typename T>
std::vector<T> filter(const std::vector<T>& vals, const std::function<bool(const T&)>& func) {
  std::vector<T> retval;
  for(auto& val : vals) {
    if (func(val)) retval.push_back(val);
  }
  return retval;
}
  

std::string topicType(const std::optional<TopicTypes>& types, const std::string& topicName) {
  if (!types) { return ""; }
  auto vs = filter<TopicTypeInfo>(types->topicTypes, [topicName](const TopicTypeInfo& v) { return v.topicName == topicName; });
  if (vs.size() > 0) { return vs[0].topicType; }
  return "";
}

void show(const std::string& str) {
  std::cout << str << std::endl;
}

template<typename T>
void show(const std::optional<T>& vals) {
  if (!vals) return;
  for(auto& n : vals.value()) {
    show(n);
  }
}


int main(const int argc, const char* argv[]) {
  ssr::nanoros::init_nanoros();
  if (argc >= 2) { 
    std::string cmd = argv[1];
    if (cmd == "list") {
      show(topicNames(getROSMaster()->getSystemState("/nanorostopic")));
    } else if (cmd == "type") {
      const std::string topicName = argv[2];      
      if (argc >= 3) show(topicType(getROSMaster()->getTopicTypes("/nanorostopic"), topicName));
    } else if (cmd == "echo") {
      if (argc >= 3) {
        const std::string topicName = argv[2];
        const std::string topicTypeName = topicType(getROSMaster()->getTopicTypes("/nanorostopic"), topicName);
        auto stub = getROSMsgStubFactory()->getStub(topicTypeName);
        auto node = registerROSNode("/nanorostopic");
        node->subscribe(topicName, stub, [](auto& topic) {
					   std::cout << topic->prettyString() << std::endl;
        });
        node->spin();
      }
    }
  }
  

  return 0;
}
