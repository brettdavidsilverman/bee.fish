#ifndef BEE_FISH__CONFIG_HPP
#define BEE_FISH__CONFIG_HPP

#define PAGE_SIZE 4096

#ifndef DEBUG
   #define DATABASE_FILENAME "/home/bee/bee.fish.data"
#else
   #define DATABASE_FILENAME "/home/bee/dev.bee.fish.data"
#endif

#endif
