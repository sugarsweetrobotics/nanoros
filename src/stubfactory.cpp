#include "nanoros/stubfactory.h"

#include <iostream>

using namespace ssr::nanoros;

std::shared_ptr<DLLProxy> StubFactoryBase::loadStubFactoryDLL(const std::string& dirName, const std::string& fileName, const std::string& funcName) {

    for (auto dirHint : stubDirHints_) {

        auto dllproxy = createDLLProxy(dirHint + "/" + dirName, fileName);
        if (!dllproxy || (dllproxy && dllproxy->failed())) {
#ifdef WIN32
            /// Try again with Build Option Directory
#ifdef _DEBUG
            dllproxy = createDLLProxy(dirHint + "/" + dirName + "/Debug", fileName);
#else
            dllproxy = createDLLProxy(dirHint + "/" + dirName + "/Release", fileName);
#endif
            if (!dllproxy) {
               return nullptr;
            }
#else
            return nullptr;
#endif
        }
        auto func = dllproxy->functionSymbol(funcName);
        if (!func) {
#ifdef WIN32
            /// Try again with Build Option Directory
#ifdef _DEBUG
            dllproxy = createDLLProxy(dirHint + "/" + dirName + "/Debug", fileName);
#else
            dllproxy = createDLLProxy(dirHint + "/" + dirName + "/Release", fileName);
#endif
            if (!dllproxy) {
                std::cout << "WARN: Can not find symbol (" << funcName << ")" << std::endl;
                return nullptr;
            }

            func = dllproxy->functionSymbol(funcName);
            if (!func) {
                std::cout << "WARN: Can not find symbol (" << funcName << ")" << std::endl;
                return nullptr;
            }
#else
            std::cout << "WARN: Can not find symbol (" << funcName << ")" << std::endl;
            return nullptr;
#endif
        }
        func(this);
        return dllproxy;
    }
}