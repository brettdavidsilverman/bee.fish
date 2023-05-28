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

   And JSON();

   auto Array() {

      
      auto blankSpace =
         Optional(BlankSpace());

      auto openBracket =
         Character("[");

      auto closeBracket =
         Character("]");

      auto value =
         Optional(LoadOnDemand(JSON));

      auto seperator =
          blankSpace and
          Character(",") and
          blankSpace;

      return
         openBracket and
         Optional(
            value  and
            Repeat(
               seperator and value,
               0
            )
         )  and
         closeBracket;

   }
}

#endif