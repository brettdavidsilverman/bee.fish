#ifndef BEE_FISH__JSON__NUMBER_HPP
#define BEE_FISH__JSON__NUMBER_HPP

#include "../Parser/Parser.hpp"

namespace BeeFishJSON {

   using namespace BeeFishParser;

   And Number() {

      auto sign =
         Character("+") or
         Character("-");

      auto integer =
         Repeat(Range("0", "9"));

      auto fraction =
         Character(".") and
         integer;

      auto exponent =
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
