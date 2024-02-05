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

      const auto decimal =
         Character(".") and
         integer;

      const auto exponent =
         (
            Character("e") or
            Character("E")
         ) and
         -sign and
         integer;
       
      const auto eof =
         Character((char)-1);
      
      const auto number =
         -sign and
         integer and
         -decimal and
         -exponent and
         -eof;

}

#endif
