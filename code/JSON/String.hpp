#ifndef BEE_FISH__JSON__STRING_HPP
#define BEE_FISH__JSON__STRING_HPP

#include "../Parser/Parser.hpp"

namespace BeeFishJSON {

   using namespace BeeFishParser;
      
   auto String() {
      auto quote = Character("\"");

      auto plainCharacter =
         not (
            Character("\\") or
            Character("\"") or
            Character("\r") or
            Character("\n") or
            Character("\b") or
            Character("\t") or
            Character("\f")
         );

      auto escapedCharacter =
         Character("\\") and (
            Character("\\") or
            Character("\"") or
            Character("r") or
            Character("n") or
            Character("b") or
            Character("t") or
            Character("f")
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
