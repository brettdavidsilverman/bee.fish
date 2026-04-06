#ifndef BEE_FISH_B_STRING__LOCALE_HPP
#define BEE_FISH_B_STRING__LOCALE_HPP

#include <unicode/uclean.h>

namespace BeeFishBString {
    
struct Locale
{
    std::locale _locale;
    
    Locale()
    {
        std::setlocale(LC_ALL, "");
        boost::locale::generator _generator;
        _locale = _generator("");

        std::locale::global(
            _locale
        );

    }

    ~Locale()
    {

        // Reset the global C++ locale to the classic "C" locale
        std::locale::global(std::locale::classic());
        u_cleanup();
        
    }

};

inline static Locale _locale;

}

#endif