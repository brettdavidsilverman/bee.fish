#ifndef BEE_FISH__JSON__STRING_HPP
#define BEE_FISH__JSON__STRING_HPP

#include "../Parser/Parser.hpp"

namespace BeeFishJSON {

   using namespace BeeFishParser;
      
      const auto quote = Character("\"");

      const auto plainCharacter =
         not (
            Character("\\") or
            Character("\"") or
            Character("\r") or
            Character("\n") or
            Character("\b") or
            Character("\t") or
            Character("\f")
         );

      const auto escapedCharacter =
         Character("\\") and (
            Character("\\") or
            Character("\"") or
            Character("r") or
            Character("n") or
            Character("b") or
            Character("t") or
            Character("f")
         );

      const auto _string =
         quote and
         Repeat(
            plainCharacter or
            escapedCharacter,
            0) and
         quote;


}

#endif
