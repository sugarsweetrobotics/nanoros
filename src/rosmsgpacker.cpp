#include "nanoros/rosmsgpacker.h"


using namespace ssr::nanoros;


std::shared_ptr<ROSMsgPacker> ROSMsgPacker::getMsgPacker(const std::string& key) {
    if (packers_.count(key) > 0) return packers_.at(key);

    auto factory = getROSMsgPackerFactory();
    auto packer = factory->getPacker(key);
    if (!packer) return nullptr;
    packers_[key] = packer;
    return packer;
}