#include "nanoros/rossrvpacker.h"


using namespace ssr::nanoros;


std::shared_ptr<ROSMsgPacker> ROSSrvPacker::getMsgPacker(const std::string& key) {
    if (stubs_.count(key) > 0) return stubs_.at(key);

    auto factory = getROSMsgPackerFactory();
    auto stub = factory->getPacker(key);
    if (!stub) return nullptr;
    stubs_[key] = stub;
    return stub;
}