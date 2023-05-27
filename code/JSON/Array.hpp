#ifndef BEE_FISH__JSON__ARRAY_HPP
#define BEE_FISH__JSON__ARRAY_HPP

#include <map>
#include <memory>

#include "../Parser/Test.hpp"

#include "Number.hpp"
#include "String.hpp"
#include "BlankSpace.hpp"
#include "Null.hpp"

namespace BeeFishJSON {

   const BeeFishParser::Parser& JSON();

   const BeeFishParser::Parser& Array() {

      using namespace BeeFishParser;

      const auto blankSpace =
         Optional(BlankSpace());

      const auto openBracket =
         Character("[");

      const auto closeBracket =
         Character("]");

      const auto value =
         LoadOnDemand(JSON);

      const auto seperator =
          blankSpace and
          Character(",") and
          blankSpace;

      const static auto array = 
         openBracket and
         Optional(
            value  and
           // Optional(
               Repeat(
                  seperator and value
               , 0)
            //)
         )  and
         closeBracket;

      return array;

   }
}

#endif