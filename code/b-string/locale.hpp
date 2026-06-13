#ifndef BEE_FISH_B_STRING__LOCALE_HPP
#define BEE_FISH_B_STRING__LOCALE_HPP

namespace BeeFishBString {
    
struct Locale
{
    Locale()
    {
       
        std::locale::global(
           std::locale("en_US.UTF-8")
        );
        
    }

    ~Locale()
    {

        // Reset the global C++ locale to the classic "C" locale
        std::locale::global(
            std::locale::classic()
        );
        
    }

};

inline static Locale locale;

}

#endif