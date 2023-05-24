#ifndef BEE_FISH_PARSER__NUMBER_HPP
#define BEE_FISH_PARSER__NUMBER_HPP

#include "Test.hpp"

namespace BeeFishParser {

   auto Integer() {
      return Repeat(Range("0", "9"));
   }

   auto Number() {
   
      const auto sign =
         Character("+") or
         Character("-");

      const auto exponent =
         (
            Character("e") or
            Character("E")
         ) and
         Optional(
            sign
         ) and
         Integer();
       
      const auto fraction =
         Character(".") and
         Integer();

      return
         Optional(sign) and
         Integer() and
         Optional(fraction) and
         Optional(exponent);
   }
}

#endif
