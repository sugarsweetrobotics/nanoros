#include <iostream>
#include <set>
#include <thread>

#include "nanoros/os.h"
#include "nanoros/nanoros.h"
#include "nanoros/rosutil.h"
#include "nanoros/rosmaster.h"
#include "nanoros/rosnode.h"
#include "nanoros/rosmsgstubfactory.h"

using namespace ssr::nanoros;


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
  ssr::nanoros::init_nanoros(argc, argv);
  if (argc >= 2) { 
    std::string cmd = argv[1];
    if (cmd == "list") {
      show(topicNames(rosmaster()->getSystemState("/nanorostopic")));
    } else if (cmd == "type") {
      const std::string topicName = argv[2];      
      if (argc >= 3) show(topicType(rosmaster()->getTopicTypes("/nanorostopic"), topicName));
    } else if (cmd == "echo") {
      if (argc >= 3) {
        const std::string topicName = argv[2];
        const std::string topicTypeName = topicType(rosmaster()->getTopicTypes("/nanorostopic"), topicName);
        auto stub = getROSMsgStubFactory()->getStub(topicTypeName);
        if (!stub) {
          std::cout << "ERROR: Topic Type Stub not found(" << topicTypeName << ")" << std::endl;
          return -1;
        }
        auto node = registerROSNode("/nanorostopic_echo");
        node->subscribe(topicName, stub, [](auto& topic) {
					   std::cout << topic->prettyString() << std::endl 
                       << "---" << std::endl;
        });
        node->spin();
      }
    } else if (cmd == "pub") {
      if (argc >= 5) {
        const std::string topicName = argv[2];
        const std::string topicTypeName = argv[3];
        const std::string topicDataStr = argv[4];
        auto stub = getROSMsgStubFactory()->getStub(topicTypeName);
        if (!stub) {
          std::cout << "ERROR: Topic Type Stub not found(" << topicTypeName << ")" << std::endl;
          return -1;
        }
        auto node = registerROSNode("/nanorostopic_pub");
        auto pub =node->advertise(topicName, stub);
        while(1) {
          std::this_thread::sleep_for(std::chrono::seconds(3));
          pub->publish(stub->fromJSON(fromJSONString(topicDataStr)));
          node->spinOnce();
        }
      }

    }
  }
  

  return 0;
}
