#ifndef BEE_FISH__JSON__CONSTANTS_HPP
#define BEE_FISH__JSON__CONSTANTS_HPP

#include "../Parser/ParserBase.hpp"

namespace BeeFishJSON {

   
   const auto _Null() {

      using namespace BeeFishParser;
     
      const auto _null =
         Word("null");
      
      return _null;

   }
}

#endif
