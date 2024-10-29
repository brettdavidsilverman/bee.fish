#ifndef ROOT_CONFIG_HPP
#define ROOT_CONFIG_HPP

#include "../Config.hpp"

#ifndef HOME_DIR
#error HOME_DIR not defined
#endif

#ifndef PAGE_SIZE
   #define PAGE_SIZE 4096
#endif

#ifndef DOMAIN
   #define DOMAIN "bee.fish"
#endif

#define DATABASE_FILENAME DATA_DIR "/" DOMAIN ".data"

#define HOST "https://" DOMAIN

#define WWW_ROOT_DIRECTORY HOME_DIR "/" DOMAIN

#define TEST_DIRECTORY WWW_ROOT_DIRECTORY "/tests"

#endif
