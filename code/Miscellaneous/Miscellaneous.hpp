#ifndef BEE_FISH__MISCELLANEOUS__HPP
#define BEE_FISH__MISCELLANEOUS__HPP

#include <string>
#include <unistd.h>
#include <memory>

#include "optional.h"
#include "../Parser/Parser.hpp"

#define ON_SUCCESS "😃"
#define ON_FAIL "🚫"

namespace BeeFishMisc {

   static int hasArg(
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

   static void outputSuccess(bool success)
   {
      if (success)
         std::cout << ON_SUCCESS << std::endl;
      else
         std::cout << ON_FAIL << std::endl;
   }

   static std::string escape(
      const std::string& input
   )
   {
      std::string output;
      for (const char& c : input) {

         switch(c) {
         case '\r':
            output += "\\r";
            break;
         case '\n':
            output += "\\n";
            break;
         case '\t':
            output += "\\t";
            break;
         case '\\':
            output += "\\\\";
            break;
         case '\"':
            output += "\\\"";
            break;
         default:
            output += c;
         }

      }

      return output;
   }

   static bool testPattern(
      BeeFishParser::Parser&
         parser,
      const std::string& pattern,
      const std::optional<bool>&
          expected)
   {
      using namespace BeeFishParser;
      bool success = true;

      cout << "\t" << escape(pattern) << ":" << flush;

      Capture capture(parser);

      capture.read(pattern);

      cout << escape(capture.value())
           << "{"
           << capture.result()
           << "}";
      success =
         capture.result() == expected;

      BeeFishMisc::outputSuccess(success);

      return success;

   }

   static bool testPattern(
      const BeeFishParser::Parser&
         parser,
      const std::string& pattern,
      const std::optional<bool>&
          expected)
   {
      using namespace BeeFishParser;

      unique_ptr<Parser> copy =
         unique_ptr<Parser>(
            parser.copy()
         );

      bool success = testPattern(
         *copy,
         pattern,
         expected
      );

      return success;
   }
   
}


   
#endif