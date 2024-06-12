#ifndef BEE_FISH__CONFIG_CODE_HPP
#define BEE_FISH__CONFIG_CODE_HPP

#ifndef DEBUG
   #define WEB_SERVER_PORT 80
   #define WEB_SERVER_HOST "bee.fish"
#else
   #define WEB_SERVER_PORT 8000
   #define WEB_SERVER_HOST "localhost"
#endif

#define TEST_SERVER_HOST "localhost"
#define TEST_SERVER_PORT 8000

#include "../Config.hpp"

#define TEMP_DIRECTORY "/var/tmp/"
#define TEMP_FILENAME TEMP_DIRECTORY DOMAIN ".data"

#define DEBUG_FILE HOME_DIR "/debug.txt"
#endif
