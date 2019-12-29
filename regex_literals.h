#pragma once
#ifndef REGEX_LITERAL_FLAGS
#define REGEX_LITERAL_FLAGS 1
#endif
#define STRINGIZE(x) STRINGIZE2(x)
#define STRINGIZE2(x) #x
#define LINE_STRING STRINGIZE(__LINE__)

#include <regex>
#include <string>
#define DEFAULT_REGEX_ENGINE std::regex_constants::ECMAScript
// Match result with string inside
template <typename CharT>
struct basic_smatch_string: public std::match_results<typename std::basic_string<CharT>::const_iterator>
{
public:
    using _string = std::basic_string<CharT>;
    basic_smatch_string() : std::match_results<typename _string::const_iterator>() {}
    basic_smatch_string(_string _s) : std::match_results<typename _string::const_iterator>(), s(_s) {}
    _string& str()
    {return s;}
    const _string& str() const
    {return s;}
protected:
    _string s;
};

using smatch_string = basic_smatch_string<char>;
using wsmatch_string = basic_smatch_string<wchar_t>;
using u16smatch_string = basic_smatch_string<char16_t>;
using u32smatch_string = basic_smatch_string<char32_t>;

template <typename CharT>
struct struct_rm : public std::basic_regex<CharT>
{
    using _string = std::basic_string<CharT>;
    using _string_view = std::basic_string_view<CharT>;
    typedef typename _string::const_iterator const_iterator; 
    typedef std::match_results<const CharT *> _cmatch;
    typedef std::match_results<const_iterator > _smatch;
    typedef basic_smatch_string<CharT> _smatch_ws;
    
    struct_rm() = default;
    struct_rm(_string str)
    {
#if REGEX_LITERAL_FLAGS
        std::regex_constants::syntax_option_type flags;
        if (check_regex_flags(str, flags))
            this->assign(str, flags);
        else
#endif
        this->assign(str);
    }
    struct_rm(const CharT *_Str) : struct_rm(_string(_Str))
    {
        
    }
    struct_rm(const CharT *_Str, size_t _Len) : struct_rm(_string(_Str, _Len))
    {
        
    }

    // Deleted method
    // To get the match results, we have to get the string reference !
    // Use match_results_ws instead to get the string into it
    inline bool match(_string&& str, _smatch& sm) = delete;
    // Deleted method
    // r-value string not allowed with smatch ! Use match_results_ws instead to save r-value string.
    inline bool search(_string&& str, _smatch& sm) = delete;

    
    inline bool match(_string&& str, _smatch_ws& sm)
    {
        sm.str().swap(str);
        return std::regex_match(sm.str(), sm, *this);
    }
    inline bool search(_string&& str, _smatch_ws& sm)
    {
        sm.str().swap(str);
        return std::regex_search(sm.str(), sm, *this);
    }
    inline bool match(const _string_view& str, _cmatch& sm)
    {
        return std::regex_match(str.data(), sm, *this);
    }
    inline bool match(const CharT* str, _cmatch& sm)
    {
        return std::regex_match(str, sm, *this);
    }
    inline bool match(const _string& str, _smatch& sm)
    {
        return std::regex_match(str, sm, *this);
    }
    inline bool match(const CharT* str)
    {
        return std::regex_match(str, *this);
    }
    inline bool match(const _string& str)
    {
        return std::regex_match(str, *this);
    }
    inline bool match(_string&& str)
    {
        return std::regex_match(str, *this);
    }
    inline bool match(_string_view str)
    {
        return std::regex_match(str.data(), *this);
    }
    inline bool search(const CharT* str, _cmatch& sm)
    {
        return std::regex_search(str, sm, *this);
    }
    inline bool search(const _string& str, _smatch& sm)
    {
        return std::regex_search(str, sm, *this);
    }
    inline bool search(const _string_view& str, _cmatch& sm)
    {
        return std::regex_search(str.data(), sm, *this);
    }
    inline bool search(const CharT* str)
    {
        return std::regex_search(str, *this);
    }
    inline bool search(const _string& str)
    {
        return std::regex_search(str, *this);
    }
    inline bool search(_string&& str)
    {
        return std::regex_search(str, *this);
    }
    inline bool search(_string_view str)
    {
        return std::regex_search(str.data(), *this);
    }
private:
#if REGEX_LITERAL_FLAGS
    inline const CharT* RegexFlagsDetector();
    //static CharT const *RegexFlagsDetector;
    bool check_regex_flags(_string& _str, std::regex_constants::syntax_option_type& _flags)
    {
        using namespace std;
        using namespace regex_constants;
        static const std::basic_regex<CharT> reg(RegexFlagsDetector());
        _smatch sm;
        syntax_option_type flags = static_cast<syntax_option_type>(0);
        syntax_option_type engine = DEFAULT_REGEX_ENGINE;
        if (regex_match(_str,sm,reg))
        {
            const auto& beg = sm[2].first, &end = sm[2].second;
            for (auto itL=beg; itL != end; itL++)
            {
                CharT l = *itL;
                switch (l)
                {
                case 'i': flags |= icase; break;
                case 'b': flags |= nosubs; break;
                case 'o': flags |= optimize; break;
                case 'c': flags |= regex_constants::collate; break;
//#if __cplusplus >= 201703L
//                case 'm': flags |= regex_constants::multiline; break;
//#endif
                case 'E': engine = ECMAScript; break;
                case 'B': engine = basic; break;
                case 'X': engine = extended; break;
                case 'A': engine = awk; break;
                case 'G': engine = grep; break;
                case 'P': engine = egrep; break;
                }
            }
            _str = sm[1];
            _flags = flags | engine;
            return true;
        }
        return false;
    }
    std::regex_constants::syntax_option_type check_regex_flags(const CharT *_Str, size_t _Len)
    {
        return check_regex_flags(_string(_Str, _Len));
    }
#endif
};

inline namespace literals
{
    inline namespace regex_literals
    {
        inline struct_rm<char> operator "" _rg(const char *_Str, size_t _Len)
        {    // construct literal from [_Str, _Str + _Len)
            return (struct_rm<char>(_Str, _Len));
        }
        inline struct_rm<wchar_t> operator "" _rg(const wchar_t *_Str, size_t _Len)
        {    // construct literal from [_Str, _Str + _Len)
            return (struct_rm<wchar_t>(_Str, _Len));
        }
        /// regex_trait<char16_t> and regex_trait<char32_t> don't exist. So impossible to define
        // inline struct_rm<char16_t> operator "" _rg(const char16_t *_Str, size_t _Len)
        // {    // construct literal from [_Str, _Str + _Len)
        //     return (struct_rm<char16_t>(_Str, _Len));
        // }
        // inline struct_rm<char32_t> operator "" _rg(const char32_t *_Str, size_t _Len)
        // {    // construct literal from [_Str, _Str + _Len)
        //     return (struct_rm<char32_t>(_Str, _Len));
        // }
    }
}
#if REGEX_LITERAL_FLAGS
template <>
inline const char* struct_rm<char>::RegexFlagsDetector() { return R"(^\/(.*)\/([ibocmEXBAGP]*)$)"; };
template <>
inline const wchar_t* struct_rm<wchar_t>::RegexFlagsDetector() { return LR"(^\/(.*)\/([ibocmEXBAGP]*)$)"; };
// template <>
// inline const char16_t* struct_rm<char16_t>::RegexFlagsDetector() { return uR"(^\/(.*)\/([ibocmEXBAGP]*)$)"; };
// template <>
// inline const char32_t* struct_rm<char32_t>::RegexFlagsDetector() { return UR"(^\/(.*)\/([ibocmEXBAGP]*)$)"; };
#endif
#undef STRINGIZE
#undef STRINGIZE2
#undef LINE_STRING
