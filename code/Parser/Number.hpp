#ifndef BEE_FISH_PARSER__NUMBER_HPP
#define BEE_FISH_PARSER__NUMBER_HPP

#include "Test.hpp"

namespace BeeFishParser {

   And NumberExample() {
   
      const auto sign =
         Character("+") or
         Character("-");

      const auto integer =
         Repeat(Range("0", "9"));

      const auto fraction =
         Character(".") and
         integer;

      const auto exponent =
         (
            Character("e") or
            Character("E")
         ) and
         Optional(
            sign
         ) and
         integer;

      return
         Optional(sign) and
         integer and
         Optional(fraction) and
         Optional(exponent);
   }
}

#endif
