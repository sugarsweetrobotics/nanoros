#pragma once

#include <functional>
#include <utility>
#include <string>
#include <memory>

#ifdef WIN32
#include <Windows.h> // For HINSTANCE
#endif

namespace ssr::nanoros {


    class DLLProxy {
    private:
        bool failed_;
        std::string m_name;
        int m_mode;
        int m_closeflag;
#ifdef WIN32
        HINSTANCE handle_;
#else
        void* handle_;
#endif
        int m_error;
        std::string dll_name_;
    public:
      DLLProxy(const std::string& name);

      DLLProxy(std::string path, const std::string& filename);
      ~DLLProxy();
      
      bool failed() { return failed_; }
      bool isNull() { return handle_ == 0; }
    public:

      std::function<void(void*)> functionSymbol(const std::string& name);
    };

    std::shared_ptr<DLLProxy> createDLLProxy(const std::string& name);

    std::shared_ptr<DLLProxy> createDLLProxy(const std::string& path, const std::string& filename);

}