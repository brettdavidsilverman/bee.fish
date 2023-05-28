#ifndef BEE_FISH__JSON__CONSTANTS_HPP
#define BEE_FISH__JSON__CONSTANTS_HPP

#include "../Parser/ParserBase.hpp"

namespace BeeFishJSON {

   using namespace BeeFishParser;
   
   auto Null() {

      return Word("null");

   }

   auto True() {

      return Word("true");

   }

   auto False() {

      return Word("false");

   }

   auto Undefined() {

      return Word("undefined");

   }
}

#endif
