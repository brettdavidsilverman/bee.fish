#ifndef BEE_FISH__JSON__OBJECT_HPP
#define BEE_FISH__JSON__OBJECT_HPP

#include <map>
#include <memory>

#include "../Parser/Test.hpp"

#include "Number.hpp"
#include "String.hpp"
#include "BlankSpace.hpp"
#include "Null.hpp"

namespace BeeFishJSON {

   const BeeFishParser::Parser& JSON();

   const BeeFishParser::Parser& Object() {

      using namespace BeeFishParser;

      const auto openBrace =
         Character("{");

      const auto closeBrace =
         Character("}");


      const auto key =
         String();

      const auto colon =
         Character(":");

      const auto value =
         LoadOnDemand(JSON);

      const auto seperator =
          Character(",");

      const auto blankSpace =
         BlankSpace();

      const auto line =
         blankSpace and
         key and blankSpace and
         colon and blankSpace and 
         value and blankSpace;

      const static auto object = 
         blankSpace and
         openBrace and blankSpace and
         Optional(
            line and blankSpace and
            Repeat(
               seperator and line,
               0
            )
         )  and
         closeBrace;

      return object;

   }
}

#endif