#ifndef BEE_FISH__JSON__NUMBER_HPP
#define BEE_FISH__JSON__NUMBER_HPP

#include "../Parser/Test.hpp"

namespace BeeFishJSON {

   const auto Number() {

      using namespace BeeFishParser;

      const auto sign =
         Character("+") or
         Character("-");

      const auto integer =
         Repeat(Range("0", "9"));

      const auto exponent =
         (
            Character("e") or
            Character("E")
         ) and
         Optional(
            sign
         ) and
         integer;
       
      const auto fraction =
         Character(".") and
         integer;

      const auto number =
         Optional(sign) and
         integer and
         Optional(fraction) and
         Optional(exponent);

      return number;
   }
}

#endif
