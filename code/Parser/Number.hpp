#ifndef BEE_FISH_PARSER__NUMBER_HPP
#define BEE_FISH_PARSER__NUMBER_HPP

#include "Test.hpp"

namespace BeeFishParser {

   auto Integer() {
      return Repeat(Range("0", "9"));
   }

   auto Number() {
   
      return
         Optional(
            Character("-")
         ) and
         Integer() and
         Optional(
            Character(".") and
            Integer()
         ) and
         Optional(
            ( Character("e") or
              Character("E") ) and
            Optional(
               Character("+") or
               Character("-")
            ) and
            Integer()
         );
   }
}

#endif
