#ifndef BEE_FISH_DATABASE__JSON_INDEX_HPP
#define BEE_FISH_DATABASE__JSON_INDEX_HPP

#include "Index.hpp"

namespace BeeFishDatabase {

    // At the root of JSONDatabase
    inline static const Index ORIGINS = 0;
    inline static const Index WORDS = 0;
    inline static const Index PROPERTIES = 1;
    inline static const Index OBJECTS = 2;
    
    // Within user data
    inline static const Index URLS = 0;
    
    // Within an object
    inline static const Index POSITIONS = 0;
    inline static const Index VALUES = 1;
    
    inline static const Index BY_KEY = 0;
    inline static const Index BY_OBJECT = 2;
}

#endif