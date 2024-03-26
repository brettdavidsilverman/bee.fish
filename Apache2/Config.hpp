#ifndef APACHE2_CONFIG_HPP
#define APACHE2_CONFIG_HPP

#include "../Config.hpp"


#ifdef DEBUG
   #define DATABASE_FILENAME "/home/bee/dev.bee.fish.data"
#else
   #define DATABASE_FILENAME "/home/bee/bee.fish.data"
#endif

#define HOST "https://bee.fish"

#define TEST_DIRECTORY "/home/bee/bee.fish/tests"

#endif
