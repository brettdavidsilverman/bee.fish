#ifndef BEE_FISH__JSON__STRING_HPP
#define BEE_FISH__JSON__STRING_HPP

#include "../Parser/Test.hpp"

namespace BeeFishJSON {

   
   auto String() {
      using namespace BeeFishParser;
      
      auto quote = Character("\"");

      auto plainCharacter =
         not (
            Character("\\") or
            Character("\"") or
            Character("\r") or
            Character("\n") or
            Character("\b") or
            Character("\f")
         );

      auto escapedCharacter =
         Character("\\") and (
            Character("\\") or
            Character("\"") or
            Character("\r") or
            Character("\n") or
            Character("\b") or
            Character("\f")
         );

      return
         quote and
         Repeat(
            plainCharacter or
            escapedCharacter,
            0) and
         quote;
   }
}

#endif
