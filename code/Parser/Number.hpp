#ifndef BEE_FISH_PARSER__NUMBER_HPP
#define BEE_FISH_PARSER__NUMBER_HPP

#include "Test.hpp"

namespace BeeFishParser {
   auto Number() {
   
      auto integers =
         Repeat(Range("0", "9"));

      return
         Optional(
            Character("-")
         ) and
         integers and
         Optional(
            Character(".") and
            integers
         ) and
         Optional(
            ( Character("e") or
              Character("E") ) and
            Optional(
               Character("+") or
               Character("-")
            ) and
            integers
         );
   }
}

#endif
