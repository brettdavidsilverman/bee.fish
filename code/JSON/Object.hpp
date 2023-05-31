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

   const And JSON();

   const auto openBrace =
      Character("{");

   const auto closeBrace =
      Character("}");

   const auto key =
      _string;

   const auto colon =
      Character(":");

   const auto value =
      LoadOnDemand<And>(JSON);

   const auto seperator =
       Character(",");

   const auto line =
      -blankSpaces and
      key and -blankSpaces and
      colon and -blankSpaces and 
      value and -blankSpaces;

   const auto object =
      -blankSpaces and
      openBrace and -blankSpaces and
      Optional(
         line and -blankSpaces and
         Repeat(
            seperator and line,
            0
         )
      ) and
      closeBrace;


}

#endif