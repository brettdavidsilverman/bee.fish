#ifndef BEE_FISH__MISCELLANEOUS__HPP
#define BEE_FISH__MISCELLANEOUS__HPP

#include <string>
#include <unistd.h>

#include "optional.h"

#define ON_SUCCESS "😃"
#define ON_FAIL "🚫"

namespace BeeFishMisc {

   inline int hasArg(
      int argc,
      const char* argv[],
      const std::string& arg
   )
   {
      for (int i = 0; i < argc; i++)
      {
         if (arg == argv[i])
            return i;
      }
   
      return -1;
   }

   static void sleep(long seconds) {
      usleep(seconds * 1000L * 1000L);
   }

   inline void outputSuccess(bool success)
   {
      if (success)
         std::cout << ON_SUCCESS << std::endl;
      else
         std::cout << ON_FAIL << std::endl;
   }

}

#endif