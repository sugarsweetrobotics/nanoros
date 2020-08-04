#pragma once

#include <string>

const std::string hdr_part[] = {
	R"(
#pragma once
#include "nanoros/rosmsg.h"
#include "nanoros/rosmsgstubfactory.h"
#include <sstream>
#include <string>
)",
R"(
namespace ssr::nanoros {
)"

};

const std::string src_part[] = {
	R"(
#include "nanoros/nanoros_define.h"

#include "nanoros/rosmsgstub.h"
#include "nanoros/rostcpros.h"
)"

};