#include "nanoros/rossrvpacker.h"


using namespace ssr::nanoros;


std::shared_ptr<ROSMsgStub> ROSSrvStub::getMsgStub(const std::string& key) {
    if (stubs_.count(key) > 0) return stubs_.at(key);

    auto factory = getROSMsgStubFactory();
    auto stub = factory->getStub(key);
    if (!stub) return nullptr;
    stubs_[key] = stub;
    return stub;
}