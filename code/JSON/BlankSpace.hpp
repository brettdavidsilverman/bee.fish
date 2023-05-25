#ifndef BEE_FISH__JSON__BLANK_SPACE_HPP
#define BEE_FISH__JSON__BLANK_SPACE_HPP

#include "../Parser/Test.hpp"

namespace BeeFishJSON {

   
   const auto BlankSpace() {
      using namespace BeeFishParser;
      
      const auto space =
         Character(" ");

      const auto lineFeed =
         Character("\n");

      const auto carriageReturn =
         Character("\r");

      const auto horizontalTab =
         Character("\t");

      const auto blankSpace =
         Repeat(
            space or
            lineFeed or
            carriageReturn or
            horizontalTab,
            0
         );

      return blankSpace;

   }
}

#endif
