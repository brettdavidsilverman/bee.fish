#ifndef BEE_FISH__MISCELLANEOUS__HPP
#define BEE_FISH__MISCELLANEOUS__HPP

#include <string>
#include <unistd.h>
#include <memory>
#include "Log.hpp"
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

   
   inline void sleep(long seconds) {
      usleep(seconds * 1000L * 1000L);
   }

   inline void outputSuccess(bool success)
   {
      if (success)
         std::cout << ON_SUCCESS << std::endl;
      else
         std::cout << ON_FAIL << std::endl;
   }

   inline std::string escape(
      char c
   )
   {
      
      switch(c) {
         case '\r':
            return "\\r";
         case '\n':
            return "\\n";
         case '\t':
            return "\\t";
         case '\\':
            return "\\\\";
         case '\"':
             return "\\\"";
         case -1:
             return "{eof}";
         default: {
            std::string str;
            str = c;
            return str;
         }
      }

      return "";
   }

   inline std::string escape(
      const std::string& input
   )
   {
      std::string output;
      for (const char& c : input) {

         output += escape(c);

      }

      return output;
   }

   
   
}


   
#endif