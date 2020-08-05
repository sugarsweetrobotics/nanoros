#pragma once

#include "nanoros/rossrvpacker.h"

#include <memory>
#include <string>
#include <functional>

namespace ssr {
    namespace nanoros {

        class ROSNode;
        class ROSServiceProvider {
        protected:
            const std::string serviceName_;
            ROSNode* node_;
        
        public:
            ROSServiceProvider(ROSNode* node, const std::string& serviceName): node_(node), serviceName_(serviceName) {}
            virtual ~ROSServiceProvider() {}

            virtual std::string getUri() const { return ""; }
            std::string getServiceName() const { return serviceName_; }
        };


        std::shared_ptr<ROSServiceProvider> createROSServiceProvider(ROSNode* node, const std::string& serviceName, const std::shared_ptr<ROSSrvPacker>& stub,
            const std::function<const std::shared_ptr<ROSSrvResponse>(const std::shared_ptr<const ROSSrvRequest>&)>& func);

        // std::shared_ptr<ROSServiceProvider> serviceProvider(const std::shared_ptr<ROSMaster>& master, const std::string& caller_id, const std::string& service_name);
    }
}