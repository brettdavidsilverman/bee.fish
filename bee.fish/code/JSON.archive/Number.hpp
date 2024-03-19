#ifndef BEE_FISH__JSON__NUMBER_HPP
#define BEE_FISH__JSON__NUMBER_HPP

#include "../Parser/Parser.hpp"
#include "BlankSpace.hpp"

namespace BeeFishJSON {

   using namespace BeeFishParser;

      const auto sign =
         Character("+") or
         Character("-");

      const auto digit =
         Range("0", "9");

      const auto digits =
         Repeat(digit);

      const auto integer =
         -sign and digits;

      const auto decimal =
         Character(".") and
         digits;

      const auto exponent =
         (
            Character("e") or
            Character("E")
         ) and
         -sign and
         digits;
       
      
      const auto number =
         -sign and
         digits and
         -decimal and
         -exponent;

}

#endif