#include "nanoros/rosmsgpackerfactory.h"
#include "nanoros/dllproxy.h"
#include "nanoros/stringutil.h"
#include <map>
#include <iostream>

using namespace ssr::nanoros;

class ROSMsgStubFactoryImpl : public ROSMsgStubFactory {



public:
  ROSMsgStubFactoryImpl() {}
  virtual ~ROSMsgStubFactoryImpl() {}


public:

  bool tryLoadStubDLL(const std::string& topicTypeName) {
    auto tokens = stringSplit(topicTypeName, '/');
    if (tokens.size() != 2) {
      return false;
    }

    auto dirName = tokens[0] + "/msg";
    auto fileName = tokens[1];
    auto funcName = "init_msg_" + tokens[0] + "_" + tokens[1];

    auto dllproxy = loadStubFactoryDLL(dirName, fileName, funcName);

    if (!dllproxy) return false;

    dllproxies_[topicTypeName] = dllproxy;
    return true;
     
  }
};

namespace {
  auto factory = std::make_shared<ROSMsgStubFactoryImpl>();
}

std::shared_ptr<ROSMsgStubFactory> ssr::nanoros::getROSMsgStubFactory() {
  return std::static_pointer_cast<ROSMsgStubFactory>(factory);
}
