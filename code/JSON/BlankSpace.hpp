#ifndef BEE_FISH__JSON__BLANK_SPACE_HPP
#define BEE_FISH__JSON__BLANK_SPACE_HPP

#include "../Parser/Parser.hpp"

namespace BeeFishJSON {
   
   using namespace BeeFishParser;
      

      const auto space =
         Character(" ");

      const auto tab =
         Character("\t");

      const auto lineFeed =
         Character("\n");

      const auto carriageReturn =
         Character("\r");

      

      const auto blank = space or tab;
      const auto blanks = Repeat(blank);

      const auto blankSpace =
         space or
         tab or
         lineFeed or
         carriageReturn;

      const auto blankSpaces =
         Repeat(blankSpace, 0);

      const auto newLine =
         Word("\r\n") or Word("\n");


}

#endif
