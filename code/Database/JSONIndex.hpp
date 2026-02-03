#ifndef BEE_FISH_DATABASE__JSON_INDEX_HPP
#define BEE_FISH_DATABASE__JSON_INDEX_HPP

#include "Index.hpp"

namespace BeeFishDatabase {

    // At the root of JSONDatabase
    inline static const Index AUTHENTICATION = 1;
    inline static const Index SECRETS = 2;
    inline static const Index USERS = 3;
    inline static const Index IP_ADDRESSES = 4;
    inline static const Index JSON = 5;
    inline static const Index PROPERTIES = 6;
    inline static const Index WORDS = 7;
    
        
    // Within user data
    inline static const Index URLS = 1;
    
    // within session data
    inline static const Index USER_ID = 1;
    inline static const Index LAST_AUTHENTICATION = 2;
    
}

#endif