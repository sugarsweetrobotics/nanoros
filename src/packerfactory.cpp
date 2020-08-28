#include "nanoros/packerfactory.h"

#include <iostream>

#include "plog/Log.h"

using namespace ssr::nanoros;

std::shared_ptr<DLLProxy> PackerFactoryBase::loadPackerFactoryDLL(const std::string& dirName, const std::string& fileName_, const std::string& funcName) {

    std::string fileName = fileName_;
#ifdef WIN32
#ifdef _DEBUG
    fileName = fileName + "d";
#endif // _DEBUG
#endif // WIN32
    PLOGD << "PackerFactoryBase::loadPackerFactoryDLL(" << dirName << ", " << fileName << ", " << funcName << ") called." ;


    for (auto dirHint : packerDirHints_) {
      if (dirHint.rfind('/') != dirHint.length() -1) {
	dirHint += '/';
      }
      PLOGV << "searching DLL with dirHint:" << dirHint;

        auto dllproxy = createDLLProxy(dirHint + dirName, fileName);
        if (!dllproxy || (dllproxy && dllproxy->failed())) {
#ifdef WIN32
            PLOGV << "createDLLProxy failed with hint: " << dirHint + "/" + dirName ;
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
            continue;
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
                //std::cout << "WARN: Can not find symbol (" << funcName << ")" ;
                //return nullptr;
                continue;
            }

            func = dllproxy->functionSymbol(funcName);
            if (!func) {
                //std::cout << "WARN: Can not find symbol (" << funcName << ")" ;
                //return nullptr;
                continue;
            }
#else
           ///std::cout << "WARN: Can not find symbol (" << funcName << ")" ;
            continue;
#endif
        }
        func(this);
        return dllproxy;
    }
    // std::cout << "WARN: Can not find symbol (" << funcName << ")" ;
    return nullptr;
}
