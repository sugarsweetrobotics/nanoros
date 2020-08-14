
#include "nanoros/rosmsg.h"
#include <nlohmann/json.hpp>


using namespace ssr::nanoros;

std::string ROSMsg::prettyString(const std::string& indent) const {
    auto j = this->toJSON();
    if (j) return j->prettyString(indent);
    return "";
}