#include "nanoros/rossrvstubfactory.h"
#include "nanoros/dllproxy.h"
#include "nanoros/stringutil.h"
#include <map>
#include <iostream>

using namespace ssr::nanoros;




class ROSSrvStubFactoryImpl : public ROSSrvStubFactory {
private:
  std::map<std::string, std::shared_ptr<DLLProxy>> dllproxies_;
  std::map<std::string, std::shared_ptr<ROSSrvStub>> stubs_;
public:
  ROSSrvStubFactoryImpl() {}
  virtual ~ROSSrvStubFactoryImpl() {}


public:

  virtual void registerStub(const std::shared_ptr<ROSSrvStub>& stub) {
    if (!stub) return;
    stubs_[stub->typeName()] = stub;
  }

  virtual std::shared_ptr<ROSSrvStub> getStub(const std::string& serviceTypeName) {
    if (stubs_.count(serviceTypeName) == 0) {
      if (tryLoadStubDLL(serviceTypeName)) {
        if (stubs_.count(serviceTypeName) == 0) return nullptr;
        return stubs_[serviceTypeName];
      }
      std::cout << "WARN: Can not load DLL (" << serviceTypeName << ")" << std::endl;
      return nullptr;
    }
    return stubs_[serviceTypeName];
  }

  bool tryLoadStubDLL(const std::string& serviceTypeName) {
    auto tokens = stringSplit(serviceTypeName, '/');
    if (tokens.size() != 2) {
      return false;
    }

    auto dllproxy = createDLLProxy(tokens[0] +"/srv", tokens[1]);
    if (!dllproxy) return false;
    auto func = dllproxy->functionSymbol("init_srv_" + tokens[0] + "_" + tokens[1]);
    if (!func)  {
      std::cout << "WARN: Can not find symbol (" << "init_srv_" + tokens[0] + "_" + tokens[1] << ")" << std::endl;
      return false;
    }
    dllproxies_[serviceTypeName] = dllproxy;
    func(this);
    return true;
     
  }
};

namespace {
  auto factory = std::make_shared<ROSSrvStubFactoryImpl>();
}

std::shared_ptr<ROSSrvStubFactory> ssr::nanoros::getROSSrvStubFactory() {
  return std::static_pointer_cast<ROSSrvStubFactory>(factory);
}
