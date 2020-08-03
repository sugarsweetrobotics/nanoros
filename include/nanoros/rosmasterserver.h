#pragma once

#include "nanoros/nanoros_define.h"

#include <memory>
#include <string>
#include <cstdint>
#include <optional>

namespace ssr {

    namespace nanoros {



        class ROSMasterServer {
        public:
            ROSMasterServer() {}
            virtual ~ROSMasterServer() {}


        public:
            virtual void start(const bool blocking = true) {}
        };


        NANOROS_API std::shared_ptr<ROSMasterServer> masterServer(std::optional<std::pair<std::string, int32_t>>& masterInfo);
    }
}