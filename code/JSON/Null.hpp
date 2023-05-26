#ifndef BEE_FISH__JSON__NULL_HPP
#define BEE_FISH__JSON__NULL_HPP

#include "../Parser/Test.hpp"

namespace BeeFishJSON {

   
   const auto Null() {

      using namespace BeeFishParser;
     
      const auto _null =
         Word("null");
      
      return _null;

   }
}

#endif
