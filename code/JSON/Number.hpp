#ifndef BEE_FISH__JSON__NUMBER_HPP
#define BEE_FISH__JSON__NUMBER_HPP

#include "../Parser/Test.hpp"

namespace BeeFishJSON {

   using namespace BeeFishParser;

   auto Integer() {
      return Range("0", "9");
   }

   auto Integers() {
      return Repeat(Integer());
   }

   auto Exponent() {
      return
         (
            Character("e") or
            Character("E") 
         ) and
         Optional(
            Character("+") or
            Character("-")
         ) and
         Integers();
   }

   auto Fractions() {
      return
         Character(".") and
         Integers();
   }

   auto Number() {

      return
         Optional(
            Character("-")
         ) and
         Integers() and
         Optional(
            Fractions()
         ) and
         Optional(
            Exponent()
         );
   }
}

#endif
