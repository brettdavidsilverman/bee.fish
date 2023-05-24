#ifndef BEE_FISH__JSON__STRING_HPP
#define BEE_FISH__JSON__STRING_HPP

#include "../Parser/Test.hpp"

namespace BeeFishJSON {

         
   auto String() {
      using namespace BeeFishParser;
      
      auto quote = Character("\"");

      auto escapedCharacter =
          Character("\\") or
          Character("\"") or
          Character("\r") or
          Character("\n") or
          Character("\b") or
          Character("\f");

      auto stringCharacter =
         not quote;// or
         //escapedCharacter;
  
      return
         quote and
         Repeat(stringCharacter, 0) and
         quote;
   }
}

#endif
