#pragma once

#include "nanoros/dllproxy.h"

#include <memory>
#include <string>
#include <map>
#include <vector>

namespace ssr {
    namespace nanoros {

        class PackerFactoryBase {
        protected:

            std::vector<std::string> packerDirHints_;
        public:
            virtual ~PackerFactoryBase() {}

            virtual std::shared_ptr<DLLProxy> loadPackerFactoryDLL(const std::string& dirName, const std::string& fileName, const std::string& funcName);


        public:
            void addPackerDirHint(const std::string& hint) { packerDirHints_.push_back(hint); }
        };


        template<typename T>
        class PackerFactory : public PackerFactoryBase {
        protected:
            std::map<std::string, std::shared_ptr<DLLProxy>> dllproxies_;
            std::map<std::string, std::shared_ptr<T>> packers_;

        public:
            PackerFactory() {
                packerDirHints_.push_back(".");
            }
            virtual ~PackerFactory() {}

        public:
            virtual void registerPacker(const std::shared_ptr<T>& packer) {
                if (!packer) return;
                packers_[packer->typeName()] = packer;
            }


            virtual bool tryLoadPackerDLL(const std::string& typeName) = 0;

            virtual std::shared_ptr<T> getPacker(const std::string& typeName) {
                if (packers_.count(typeName) == 0) {
                    if (tryLoadPackerDLL(typeName)) {
                        if (packers_.count(typeName) == 0) return nullptr;
                        return packers_[typeName];
                    }
                    // std::cout << "WARN: Can not load DLL (" << serviceTypeName << ")" << std::endl;
                    return nullptr;
                }
                return packers_[typeName];
            }
        public:

            
        };
    }
}