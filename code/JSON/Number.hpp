#ifndef BEE_FISH__JSON__NUMBER_HPP
#define BEE_FISH__JSON__NUMBER_HPP

#include "../Parser/Parser.hpp"

namespace BeeFishJSON {

   using namespace BeeFishParser;

      const auto sign =
         Character("+") or
         Character("-");

      const auto digit =
         Range("0", "9");

      const auto digits =
         Repeat(digit);

      const auto integer = digits;

      const auto fraction =
         Character(".") and
         integer;

      const auto exponent =
         (
            Character("e") or
            Character("E")
         ) and
         -sign and
         integer;
       
      
      const auto number =
         -sign and
         integer and
         -fraction and
         -exponent;

}

#endif
