
#include "nanoros/rosmsg.h"
#include <nlohmann/json.hpp>

#include "plog/Log.h"

using namespace ssr::nanoros;

std::string ROSMsg::prettyString(const std::string& indent) const {
  
  PLOGV << "ROSMsg::prettyString(indent=\"" << indent << "\" called";
    auto j = this->toJSON();
    PLOGV << " - JSON string is '" << j << "'";
    if (j) return j->prettyString(indent);
    return "";
}
