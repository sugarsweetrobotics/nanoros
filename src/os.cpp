#include <iostream>
#include <stdlib.h>
#include <sstream>


#ifdef WIN32

#else
#include <unistd.h> /* environ */
#endif
#include "nanoros/stringutil.h"
#include "nanoros/os.h"


std::string ssr::nanoros::getEnv(const std::string& key) {

#ifdef WIN32
    auto buf = std::getenv(key.c_str());
    if (buf == nullptr) return "";
    return buf;
#else
    auto buf = getenv(key.c_str());
    if (buf == nullptr) return "";
    return buf;
#endif
}



std::string ssr::nanoros::getExecutablePath(const std::string& path) {
#ifdef WIN32

    TCHAR pathBuf[MAX_PATH];   //パス取得用
    if (::GetModuleFileName(NULL, pathBuf, MAX_PATH)) {
        auto pathValue = replaceAll(pathBuf, "\\", "/");
        auto pos = pathValue.rfind('/');
        auto absPath = pathValue.substr(0, pos + 1);
        return absPath;
    }
    return "";
#else
    const char sep = '/';
    if (path.find(sep) == 0) return path; // Absolute Path

    auto envPath = getEnv("PATH");

    auto ps = stringSplit(envPath, ':');
    ps.push_back(getCwd());
    for (auto& p : ps) {
        auto a = p;
        if (p[p.length() - 1] != sep) {
            a = a + sep;
        }
        a = a + path;
        if (fopen(a.c_str(), "r") != NULL) {
            return simplifyPath(a);
        }
    }
    return simplifyPath(path);
#endif

}

/*

std::string ssr::nanoros::getExecutablePath(const std::string& path) {
    const char sep = '/';
    if (path.find(sep) == 0) return path; // Absolute Path
    auto ps = stringSplit(getEnv("PATH"), ':');
    ps.push_back(getCwd());
    for(auto& p : ps) {
        auto a = p;
        if (p[p.length()-1] != sep) {
            a = a + sep;
        }
        a = a + path;
        if (fopen(a.c_str(), "r") != NULL) {
            return simplifyPath(a);
        }
    }
    return simplifyPath(path);
}
*/

std::string ssr::nanoros::simplifyPath(const std::string& path) {
    const char sep = '/';
    auto ns = stringSplit(path, sep);
    std::stringstream ss;
    for(int i = 1;i < ns.size();i++) {
       // std::cout << "ns:" << ns[i-1] << "," << ns[i] << std::endl;
        
        if (ns[i] == "..") {
            i++;
        } else {
            ss << ns[i-1] << "/";
        }
    }
    if (ns[ns.size()-1] != "..") {
        ss << ns[ns.size()-1];
    }

    if (path[path.length() -1] == '/') ss << '/';
    if (path[0] == '/' && ss.str()[0] != '/') return '/' + ss.str();
    return ss.str();
}
