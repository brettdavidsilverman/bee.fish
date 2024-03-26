#ifndef BEE_FISH__CONFIG_HPP
#define BEE_FISH__CONFIG_HPP

#include "../Config.hpp"


#ifndef DEBUG
   #define DATABASE_FILENAME HOME_DIR "/bee.fish.data"
#else
   #define DATABASE_FILENAME HOME_DIR "/dev.bee.fish.data"
#endif

#ifndef HOST
#define HOST "https://bee.fish"
#endif

#define TEST_DIRECTORY HOME_DIR "/bee.fish/tests"

#endif
