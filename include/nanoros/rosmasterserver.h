#pragma once



#include "nanoros/nanoros_define.h"
#include "nanoros/nanoros.h"

#include <memory>
#include <string>
#include <cstdint>
#include <optional>
#include <thread>   
#include <iostream>

namespace ssr {

    namespace nanoros {



        class ROSMasterServer {
        public:
            ROSMasterServer() {}
            virtual ~ROSMasterServer() {}


        public:
            virtual void start(const bool blocking = true) {}

            virtual void spinOnce() {

            }

	  virtual void spin() {} 
        };


        NANOROS_API std::shared_ptr<ROSMasterServer> masterServer(const std::optional<std::pair<std::string, int32_t>>& masterInfo);
    }
}
