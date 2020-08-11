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


        std::shared_ptr<ROSServiceProvider> createROSServiceProvider(ROSNode* node, const std::string& serviceName, const std::shared_ptr<ROSSrvPacker>& packer,
            const std::function<const std::shared_ptr<ROSMsg>(const std::shared_ptr<const ROSMsg>&)>& func);
    }
}