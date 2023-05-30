#ifndef BEE_FISH__JSON__CONSTANTS_HPP
#define BEE_FISH__JSON__CONSTANTS_HPP

#include "../Parser/ParserBase.hpp"

namespace BeeFishJSON {

   using namespace BeeFishParser;
   
   Word Null() {

      return Word("null");

   }

   Word True() {

      return Word("true");

   }

   Word False() {

      return Word("false");

   }

   Word Undefined() {

      return Word("undefined");

   }
}

#endif
