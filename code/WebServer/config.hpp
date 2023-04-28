#ifndef BEE_FISH__WEB_SERVER__CONFIG__HPP
#define BEE_FISH__WEB_SERVER__CONFIG__HPP

#include "../config.hpp"

#define WEB_SERVER "Version 0.0.2 Tests"

#define WEB_SERVER_PORT    80
#define WEB_SERVER_THREADS 10

#ifndef DEBUG
   #define WEB_SERVER_HOST "http://bee.fish"
#else
   #define WEB_SERVER_HOST "http://dev.bee.fish"
#endif

#endif