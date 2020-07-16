#pragma once

#include <string>
#ifdef WIN32
#include <Windows.h>

#else
#include <unistd.h>

#endif


namespace ssr {
  namespace nanoros {


    inline std::string getCwd() { 
        #ifdef WIN32
            char cdir[255];
            if (GetCurrentDirectory(255,cdir) == 0) {
                return "";
            }
            return cdir;

        #else
            char dir[256];
            if (getcwd(dir, 256) == nullptr) {
                return "";
            }
            return dir;
        #endif
    }


    std::string getEnv(const std::string& key);

    std::string getExecutablePath(const std::string& path);

    std::string simplifyPath(const std::string& path);
  }
}
