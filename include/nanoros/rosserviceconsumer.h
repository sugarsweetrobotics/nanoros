#pragma once

#include "nanoros/rosmaster.h"
#include "nanoros/rossrv.h"
#include "nanoros/rossrvstub.h"

#include <string>
#include <optional>
#include <memory>

namespace ssr {
    namespace nanoros {


        class ROSServiceConsumer {
        private:
            const std::string callerId_;
            const std::string serviceName_;
        public:
            std::string serviceName() const { return serviceName_; }
            std::string callerId() const { return callerId_; }
        public:
            ROSServiceConsumer(const std::string& caller_id, const std::string& serviceName): callerId_(caller_id), serviceName_(serviceName) {}
            virtual ~ROSServiceConsumer() {}


        public:
            virtual std::optional<std::string> typeName() const { return std::nullopt; }

            virtual std::shared_ptr<const ROSSrvResponse> call(const std::shared_ptr<ROSSrvStub>& stub, const std::shared_ptr<const ROSSrvRequest>& arg, const double timeout=1.0) { return nullptr; }
        };


        std::shared_ptr<ROSServiceConsumer> serviceConsumer(const std::shared_ptr<ROSMaster>& master, const std::string& caller_id, const std::string& service_name);

        inline std::shared_ptr<ROSServiceConsumer> serviceConsumer(const std::string& caller_id, const std::string& service_name) {
            return serviceConsumer(rosmaster(), caller_id, service_name);
        }
    }
}