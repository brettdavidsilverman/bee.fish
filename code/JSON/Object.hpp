#ifndef BEE_FISH__JSON__OBJECT_HPP
#define BEE_FISH__JSON__OBJECT_HPP

#include <map>
#include <memory>

#include "../Parser/Parser.hpp"

#include "Number.hpp"
#include "String.hpp"
#include "BlankSpace.hpp"

namespace BeeFishJSON {

   using namespace BeeFishParser;

   And JSON();

   And Object() {

      auto openBrace =
         Character("{");

      auto closeBrace =
         Character("}");

      auto key =
         String();

      auto colon =
         Character(":");

      auto value =
         LoadOnDemand(JSON);

      auto seperator =
          Character(",");

      auto blankSpace =
         BlankSpace();

      auto line =
         blankSpace and
         key and blankSpace and
         colon and blankSpace and 
         value and blankSpace;

      return
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

   }

}

#endif