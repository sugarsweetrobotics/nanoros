#include <iostream>
#include <set>
#include <thread>

#include "nanoros/os.h"
#include "nanoros/nanoros.h"
#include "nanoros/rosutil.h"
#include "nanoros/rosmaster.h"
#include "nanoros/rosnode.h"
#include "nanoros/rossrvpackerfactory.h"
#include "nanoros/rosserviceconsumer.h"


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

std::optional<std::set<std::string>> serviceNames(const std::optional<SystemState>& state) {
  if (!state) {return std::nullopt;}
  std::set<std::string> serviceNameSet;
  for(auto& tpi : state->systemState.services) {
    serviceNameSet.insert(tpi.serviceName);
  }
  return serviceNameSet;
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


//std::string serviceType(const std::optional<ServiceTypes>& types, const std::string& topicName) {
//  if (!types) { return ""; }
//  auto vs = filter<ServiceTypeInfo>(types->topicTypes, [topicName](const TopicTypeInfo& v) { return v.topicName == topicName; });
//  if (vs.size() > 0) { return vs[0].topicType; }
//  return "";/
//}

int main(const int argc, const char* argv[]) {
  ssr::nanoros::init_nanoros(argc, argv);
  if (argc >= 2) { 
    std::string cmd = argv[1];
    if (cmd == "list") {
      show(serviceNames(getROSMaster()->getSystemState("/nanorosservice_list")));
    } else if (cmd == "type") {
      if (argc == 2) return -1;
      const std::string serviceName = argv[2];
      auto srv = serviceConsumer("nanorosservice_type", serviceName);
      auto tn = srv->typeName();
      if (!tn) return -1;
      show(tn.value());
    } else if (cmd == "call") {
      if (argc < 3) return -1;
      const std::string serviceName = argv[2];
      auto srv = serviceConsumer("nanorosservice_call", serviceName);
      auto tn = srv->typeName();
      if (!tn) return -1;
      const std::string dataStr = argc >= 4 ? argv[3] : "{}";
      auto packer = getROSSrvPackerFactory()->getPacker(tn.value());
      if (!packer) {
        std::cout << "ERROR: Service Type Packer not found(" << tn.value() << ")" << std::endl;
        return -1;
      }
      auto arg = packer->fromJSON(fromJSONString(dataStr));
      auto result = srv->call(packer, arg);
      if (!result) return -1;
      std::cout << result->prettyString() << std::ends;
    }
  }
  return 0;
}


