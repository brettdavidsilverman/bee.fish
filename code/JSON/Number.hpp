#ifndef BEE_FISH__JSON__NUMBER_HPP
#define BEE_FISH__JSON__NUMBER_HPP

#include "../Parser/Test.hpp"

namespace BeeFishJSON {

   auto Integer() {
      using namespace BeeFishParser;

      return Repeat(Range("0", "9"));
   }

   auto Number() {
      using namespace BeeFishParser;

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
