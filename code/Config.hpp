#ifndef BEE_FISH__CODE__CONFIG_HPP
#define BEE_FISH__CODE__CONFIG_HPP

#include "../Config.hpp"

#ifndef DEBUG
   #define HOST "bee.fish"
   #define WEB_SERVER_PORT 80
#else
   #define HOST "dev.bee.fish"
   #define WEB_SERVER_PORT 8080
#endif

#define WEB_SERVER_HOST "http://" HOST

#endif