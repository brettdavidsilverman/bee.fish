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

   Parser* _JSON(Parser*);

   const auto openBracket =
      Character("[");

   const auto closeBracket =
      Character("]");

   const auto arraySeperator =
      Character(",");

   const auto arrayItem =
      LoadOnDemand(_JSON);

   const auto arrayItems =
      arrayItem and
      Repeat(
         blankSpaces and
         arraySeperator and
         blankSpaces and
         arrayItem,
         0
      );

   const auto array =
      openBracket and
         blankSpaces and
         Optional(
            arrayItems
         ) and
         blankSpaces and
      closeBracket;

}

#endif