#include "nanoros/rosmsgstubfactory.h"
#include "nanoros/dllproxy.h"
#include "nanoros/stringutil.h"
#include <map>
#include <iostream>

using namespace ssr::nanoros;




class ROSMsgStubFactoryImpl : public ROSMsgStubFactory {
private:
  std::map<std::string, std::shared_ptr<DLLProxy>> dllproxies_;
  std::map<std::string, std::shared_ptr<ROSMsgStub>> stubs_;
public:
  ROSMsgStubFactoryImpl() {}
  virtual ~ROSMsgStubFactoryImpl() {}


public:

  virtual void registerStub(const std::shared_ptr<ROSMsgStub>& stub) {
    if (!stub) return;
    stubs_[stub->typeName()] = stub;
  }

  virtual std::shared_ptr<ROSMsgStub> getStub(const std::string& topicTypeName) {
    if (stubs_.count(topicTypeName) == 0) {
      if (tryLoadStubDLL(topicTypeName)) {
        if (stubs_.count(topicTypeName) == 0) return nullptr;
        return stubs_[topicTypeName];
      }
      std::cout << "WARN: Can not load DLL (" << topicTypeName << ")" << std::endl;
      return nullptr;
    }
    return stubs_[topicTypeName];
  }

  bool tryLoadStubDLL(const std::string& topicTypeName) {
    auto tokens = stringSplit(topicTypeName, '/');
    if (tokens.size() != 2) {
      return false;
    }

    auto dllproxy = createDLLProxy(tokens[0], tokens[1]);
    if (!dllproxy) return false;
    auto func = dllproxy->functionSymbol("init_" + tokens[0] + "_" + tokens[1]);
    if (!func)  {
      std::cout << "WARN: Can not find symbol (" << "init_" + tokens[0] + "_" + tokens[1] << ")" << std::endl;
      return false;
    }
    dllproxies_[topicTypeName] = dllproxy;
    func(this);
    return true;
     
  }
};

namespace {
  auto factory = std::make_shared<ROSMsgStubFactoryImpl>();
}

std::shared_ptr<ROSMsgStubFactory> ssr::nanoros::getROSMsgStubFactory() {
  return std::static_pointer_cast<ROSMsgStubFactory>(factory);
}
