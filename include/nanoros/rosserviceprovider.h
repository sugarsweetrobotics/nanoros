#pragma once

#include <memory>

namespace ssr {
    namespace nanoros {


        class ROSServiceProvider {
            
        };

        std::shared_ptr<ROSServiceProvider> serviceProvider(const std::string& service_name);
    }
}