#ifndef BEE_FISH__JSON__ARRAY_HPP
#define BEE_FISH__JSON__ARRAY_HPP

#include <map>
#include <memory>

#include "../Parser/Parser.hpp"

#include "Number.hpp"
#include "String.hpp"
#include "BlankSpace.hpp"

namespace BeeFishJSON {

   using namespace BeeFishParser;

   const And JSON();

   const auto openBracket =
      Character("[");

   const auto closeBracket =
      Character("]");

   const auto arraySeperator =
       -blankSpaces and
       Character(",") and
       -blankSpaces;

   const auto array =
         openBracket and
         -(
            -value  and
            Repeat(
               arraySeperator and -value,
               0
            )
         )  and
         closeBracket;

}

#endif