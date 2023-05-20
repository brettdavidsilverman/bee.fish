#ifndef BEE_FISH__WEB_SERVER__CONFIG__HPP
#define BEE_FISH__WEB_SERVER__CONFIG__HPP

#include "../config.hpp"

#define WEB_SERVER "WebServer Version 0.0.3 Test ports"

#define WEB_SERVER_THREADS 10

#ifndef DEBUG
   #define WEB_SERVER_HOST "http://bee.fish"
   #define WEB_SERVER_PORT 80
#else
   #define WEB_SERVER_HOST "http://dev.bee.fish"
   #define WEB_SERVER_PORT 8080
#endif

#endif