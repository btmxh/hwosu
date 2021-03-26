#pragma once
#include <string>
#include <type_traits>

namespace hwo {
    template<typename _Str, typename _OStr = std::string>
    _OStr Trim(const _Str& str) {
        static_assert(!std::is_pointer_v<_Str>, "_Str can't be char*/const char*/etc.");
        _OStr s = str;
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
            return !std::isspace(ch);
        }));
        s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
            return !std::isspace(ch);
        }).base(), s.end());
        return s;
    }

    template<typename _Str1, typename _Str2>
    bool StartsWith(const _Str1& s, const _Str2& prefix) {
        static_assert(!std::is_pointer_v<_Str1>, "_Str1 can't be char*/const char*/etc.");
        return s.rfind(prefix, 0) == 0;
    } 

    template<typename _Str, typename _Func>
    void Tokenize(const _Str& s, char split, _Func f) {
        static_assert(std::is_invocable_r_v<void, decltype(f), std::string&>, "_Func is not a valid tokenize function");
        std::string token;
        for (char c : s) {
            if (c != split)
                token += c;
            else {
                if (token.length()) f(token);
                token.clear();
            }
        }

        if (token.length()) f(token);
    }
}
