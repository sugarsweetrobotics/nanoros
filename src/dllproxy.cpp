
#include "nanoros/dllproxy.h"
#ifdef WIN32
#include <Windows.h>
#else
#include <dlfcn.h>
#endif


#include "plog/Log.h"

using namespace ssr::nanoros;

DLLProxy::DLLProxy(const std::string& name) {
#ifdef WIN32
    dll_name_ = name + ".dll";
    
    if (!(handle_ = ::LoadLibraryEx(dll_name_.c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH))) {
#elif linux
    dll_name_ = name + ".so";
    if (!(handle_ = ::dlopen(dll_name_.c_str(), RTLD_LAZY))) {
#else
    dll_name_ = name + ".dylib";
    if (!(handle_ = ::dlopen(dll_name_.c_str(), RTLD_LAZY))) {
#endif
        failed_ = true;
    }
    failed_ = false;
}

DLLProxy::DLLProxy(std::string path, const std::string& name) {
    if (path.rfind("/") != path.length()) path = path + "/";
#ifdef WIN32
    dll_name_ = path + name + ".dll";
    PLOGV << "DLLProxy::DllProxy(" << path << ", " << name << ") - loadLibrary(" << dll_name_ << ")";
    if (!(handle_ = ::LoadLibraryEx(dll_name_.c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH))) {
#elif linux
    dll_name_ = path + name + ".so";
    PLOGV << "DLLProxy::DllProxy(" << path << ", " << name << ") - loadLibrary(" << dll_name_ << ")";
    if (!(handle_ = ::dlopen(dll_name_.c_str(), RTLD_LAZY))) {
#else
    dll_name_ = path + name + ".dylib";
    PLOGV << "DLLProxy::DllProxy(" << path << ", " << name << ") - loadLibrary(" << dll_name_ << ")";    
    if (!(handle_ = ::dlopen(dll_name_.c_str(), RTLD_LAZY))) {
#endif
      PLOGV << " - loadLibrary failed." << std::endl;
        failed_ =true;
    }
    failed_ = false;
}

DLLProxy::~DLLProxy() {
    if (handle_) {
#if WIN32
        ::FreeLibrary(handle_);
#else
        ::dlclose(handle_); handle_ = nullptr;
#endif
    }
}

std::function<void(void*)> DLLProxy::functionSymbol(const std::string& name) {
    if (!handle_) return nullptr;
#if WIN32
    return reinterpret_cast<void(*)(void*)>(::GetProcAddress(handle_, name.c_str()) );

#else
    auto sym = ::dlsym(handle_, name.c_str());
    if (!sym) return nullptr;
    return reinterpret_cast<void(*)(void*)>(sym);
#endif
 }


std::shared_ptr<DLLProxy> ssr::nanoros::createDLLProxy(const std::string& name) {
    return std::make_shared<DLLProxy>(name);
}

std::shared_ptr<DLLProxy> ssr::nanoros::createDLLProxy(const std::string& path, const std::string& name) {
    auto ptr = std::make_shared<DLLProxy>(path, name);
    if (ptr->failed()) return nullptr;
    return ptr;
}
