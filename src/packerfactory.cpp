#include "nanoros/packerfactory.h"

#include <iostream>

using namespace ssr::nanoros;

std::shared_ptr<DLLProxy> PackerFactoryBase::loadPackerFactoryDLL(const std::string& dirName, const std::string& fileName, const std::string& funcName) {

    for (auto dirHint : packerDirHints_) {

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
                continue;
            }
#else
            continue;/
            //return nullptr;
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
                //return nullptr;
                continue;
            }

            func = dllproxy->functionSymbol(funcName);
            if (!func) {
                std::cout << "WARN: Can not find symbol (" << funcName << ")" << std::endl;
                //return nullptr;
                continue;
            }
#else
            std::cout << "WARN: Can not find symbol (" << funcName << ")" << std::endl;
            return nullptr;
#endif
        }
        func(this);
        return dllproxy;
    }
    std::cout << "WARN: Can not find symbol (" << funcName << ")" << std::endl;
    return nullptr;
}