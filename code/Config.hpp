#ifndef BEE_FISH__CONFIG_CODE_HPP
#define BEE_FISH__CONFIG_CODE_HPP

#ifndef DEBUG
   #define WEB_SERVER_PORT 80
   #define WEB_SERVER_HOST "bee.fish"
#else
   #define WEB_SERVER_PORT 8080
   #define WEB_SERVER_HOST "dev.bee.fish"
#endif

#include "../Config.hpp"

#endif
