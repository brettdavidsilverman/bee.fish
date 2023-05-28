#ifndef BEE_FISH__JSON__BLANK_SPACE_HPP
#define BEE_FISH__JSON__BLANK_SPACE_HPP

#include "../Parser/Parser.hpp"

namespace BeeFishJSON {
   
   using namespace BeeFishParser;
      
   auto BlankSpace() {

      auto space =
         Character(" ");

      auto lineFeed =
         Character("\n");

      auto carriageReturn =
         Character("\r");

      auto horizontalTab =
         Character("\t");

      return
         Repeat(
            space or
            lineFeed or
            carriageReturn or
            horizontalTab, 0
         );


   }

}

#endif
