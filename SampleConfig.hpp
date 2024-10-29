/* Save this to the parent directory
and rename as Config.hpp
of the website and update HOME_DIR */

#define HOME_DIR "/home/bee/bee.fish"
#ifdef DEBUG
   #define DOMAIN "dev.bee.fish"
#else
   #define DOMAIN "bee.fish"
#endif
#define DATA_DIR HOME_DIR "/data"
#define PAGE_SIZE 4096
#define SERVER

