#include "nanoros/rosmsgpackerfactory.h"
#include "nanoros/dllproxy.h"
#include "nanoros/stringutil.h"
#include <map>
#include <iostream>

#include "plog/Log.h"

using namespace ssr::nanoros;

class ROSMsgPackerFactoryImpl : public ROSMsgPackerFactory {



public:
  ROSMsgPackerFactoryImpl() {}
  virtual ~ROSMsgPackerFactoryImpl() {}


public:

    bool tryLoadPackerDLL(const std::string& topicTypeName) {
        PLOGD << "ROSMsgPackerFactoryImpl::tryLoadPackerDLL(" << topicTypeName << ") called" ;
        auto tokens = stringSplit(topicTypeName, '/');
        if (tokens.size() != 2) {
            return false;
        }

        auto dirName = tokens[0] + "/msg";
        auto fileName = tokens[1];
        auto funcName = "init_msg_" + tokens[0] + "_" + tokens[1];

        auto dllproxy = loadPackerFactoryDLL(dirName, fileName, funcName);

        if (!dllproxy) {
            PLOGE << "ROSMsgPackerFactoryImpl::tryLoadPackerDLL(" << topicTypeName << ") failed." ;
            return false;
        }
        dllproxies_[topicTypeName] = dllproxy;
        return true;
    }
};

namespace {
  auto factory = std::make_shared<ROSMsgPackerFactoryImpl>();
}

std::shared_ptr<ROSMsgPackerFactory> ssr::nanoros::getROSMsgPackerFactory() {
  return std::static_pointer_cast<ROSMsgPackerFactory>(factory);
}
