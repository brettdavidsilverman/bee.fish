#ifndef BEE_FISH__CODE__CONFIG_HPP
#define BEE_FISH__CODE__CONFIG_HPP

#include "../Config.hpp"

#ifndef DEBUG
   #define WEB_SERVER_PORT 80
   #define WEB_SERVER_HOST "bee.fish"
#else
   #define WEB_SERVER_PORT 8080
   #define WEB_SERVER_HOST "dev.bee.fish"
#endif

#endif