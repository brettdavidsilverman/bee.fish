#ifndef BEE_FISH__WEB__URL_HPP
#define BEE_FISH__WEB__URL_HPP
#include "../Parser/ParserBase.hpp"

namespace BeeFishWeb {

   using namespace BeeFishParser;
   using namespace BeeFishJSON;

   auto URL(std::string& path, Invoke::Function onpath) {
      const auto seperator =
         Character("/") or
         Character("?");

      const auto url =
         Repeat(
            seperator and
            Invoke(
               Capture(
                  Repeat(
                     not (seperator or blankSpace),
                     0
                  ),
                  path
               ),
               onpath
            ),
            1
         ) and Optional(seperator);

      return url;

    }

}

#endif
