#pragma once

#include "nanoros/dllproxy.h"

#include <memory>
#include <string>
#include <map>


namespace ssr {
    namespace nanoros {

        class StubFactoryBase {
        protected:

            std::vector<std::string> stubDirHints_;
        public:
            virtual ~StubFactoryBase() {}

            virtual std::shared_ptr<DLLProxy> loadStubFactoryDLL(const std::string& dirName, const std::string& fileName, const std::string& funcName);


        public:
            void addStubDirHint(const std::string& hint) { stubDirHints_.push_back(hint); }
        };


        template<typename T>
        class StubFactory : public StubFactoryBase {
        protected:
            std::map<std::string, std::shared_ptr<T>> stubs_;
            std::map<std::string, std::shared_ptr<DLLProxy>> dllproxies_;

        public:
            StubFactory() {
                stubDirHints_.push_back(".");
            }
            virtual ~StubFactory() {}

        public:
            virtual void registerStub(const std::shared_ptr<T>& stub) {
                if (!stub) return;
                stubs_[stub->typeName()] = stub;
            }


            virtual bool tryLoadStubDLL(const std::string& typeName) = 0;

            virtual std::shared_ptr<T> getStub(const std::string& typeName) {
                if (stubs_.count(typeName) == 0) {
                    if (tryLoadStubDLL(typeName)) {
                        if (stubs_.count(typeName) == 0) return nullptr;
                        return stubs_[typeName];
                    }
                    // std::cout << "WARN: Can not load DLL (" << serviceTypeName << ")" << std::endl;
                    return nullptr;
                }
                return stubs_[typeName];
            }
        public:

            
        };
    }
}