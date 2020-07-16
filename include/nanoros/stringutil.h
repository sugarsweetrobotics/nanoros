#pragma once

#include <vector>
#include <string>

namespace ssr {
  namespace nanoros {

    /**
     * 文字列内の特定文字列の置き換え
     * @param ss 置き換え元
     * @param p パターン
     * @param m 置き換え文字列
     * @return 置き換え後の文字列
     */
    inline std::string replaceAll(const std::string ss, const std::string& p, const std::string& m) {
        auto s = ss; // コピーする
        auto pos = s.find(p);
        int toLen = m.length();
    
        // もしパターンが空の文字列ならば何もしない
        if (p.empty()) { return s;}
    
        while ((pos = s.find(p, pos)) != std::string::npos) {
            s.replace(pos, p.length(), m);
            pos += toLen;
        }
        return s;
    }

    inline std::vector<std::string> stringSplit(const std::string& str, const char sep=' ') {
        std::vector<std::string> result;
        int i = 0;
        do {
            const int begin = i;
            ///const char *begin = pstr;
            while(str[i] != sep && str[i] != 0)
                i++;

            result.push_back(str.substr(begin, i-begin));//std::string(begin, pstr));
        } while (0 != str[i++]);
        return result;
    }
  }
}
