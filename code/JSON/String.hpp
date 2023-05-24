#ifndef BEE_FISH__JSON__STRING_HPP
#define BEE_FISH__JSON__STRING_HPP

#include "../Parser/Test.hpp"

namespace BeeFishJSON {

   auto Quote() {
      using namespace BeeFishParser;

      return Character("\"");

   }

   auto PlainCharacter() {

       using namespace BeeFishParser;

       return not (
          Character("\\") or
          Character("\"") or
          Character("\r") or
          Character("\n") or
          Character("\b") or
          Character("\f")
      );
   }

   auto String() {
      using namespace BeeFishParser;

      return
         Quote() and
         Repeat(PlainCharacter(), 0) and
/*
               Character("\\") and (
                  Quote() or
                  Character("r") or
                  Character("n") or
               ) and not Quote()
            ) and
*/
         Quote();
   }
}

#endif
