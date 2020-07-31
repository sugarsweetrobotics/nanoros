#include "nanoros/rossrvstubfactory.h"
#include "nanoros/dllproxy.h"
#include "nanoros/stringutil.h"
#include <map>
#include <iostream>

using namespace ssr::nanoros;




class ROSSrvStubFactoryImpl : public ROSSrvStubFactory {
public:
  ROSSrvStubFactoryImpl() {}
  virtual ~ROSSrvStubFactoryImpl() {}


public:

  virtual bool tryLoadStubDLL(const std::string& serviceTypeName) {
    auto tokens = stringSplit(serviceTypeName, '/');
    if (tokens.size() != 2) {
      return false;
    }

    auto dirName = tokens[0] + "/srv";
    auto fileName = tokens[1];
    auto funcName = "init_srv_" + tokens[0] + "_" + tokens[1];

    auto dllproxy = loadStubFactoryDLL(dirName, fileName, funcName);
    if (dllproxy) {
        dllproxies_[serviceTypeName] = dllproxy;
        return true;
    }
    return false;
  }
};

namespace {
  auto factory = std::make_shared<ROSSrvStubFactoryImpl>();
}

std::shared_ptr<ROSSrvStubFactory> ssr::nanoros::getROSSrvStubFactory() {
  return std::static_pointer_cast<ROSSrvStubFactory>(factory);
}
