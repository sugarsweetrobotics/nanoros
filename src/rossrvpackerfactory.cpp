#include "nanoros/rossrvpackerfactory.h"
#include "nanoros/dllproxy.h"
#include "nanoros/stringutil.h"
#include <map>
#include <iostream>

using namespace ssr::nanoros;




class ROSSrvPackerFactoryImpl : public ROSSrvPackerFactory {
public:
  ROSSrvPackerFactoryImpl() {}
  virtual ~ROSSrvPackerFactoryImpl() {}


public:

  virtual bool tryLoadPackerDLL(const std::string& serviceTypeName) {
    auto tokens = stringSplit(serviceTypeName, '/');
    if (tokens.size() != 2) {
      return false;
    }

    auto dirName = tokens[0] + "/srv";
    auto fileName = tokens[1];
    auto funcName = "init_srv_" + tokens[0] + "_" + tokens[1];

    auto dllproxy = loadPackerFactoryDLL(dirName, fileName, funcName);
    if (dllproxy) {
        dllproxies_[serviceTypeName] = dllproxy;
        return true;
    }
    return false;
  }
};

namespace {
  auto factory = std::make_shared<ROSSrvPackerFactoryImpl>();
}

std::shared_ptr<ROSSrvPackerFactory> ssr::nanoros::getROSSrvPackerFactory() {
  return std::static_pointer_cast<ROSSrvPackerFactory>(factory);
}
