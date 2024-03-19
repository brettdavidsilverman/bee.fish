#ifndef BEE_FISH_MISC__PAGESIZE
#define BEE_FISH_MISC__PAGESIZE

#ifdef SERVER
#include <unistd.h>
inline int getPageSize() {
   return getpagesize();
}
#else
inline int getPageSize() {
    return 1024;
}
#endif

#endif