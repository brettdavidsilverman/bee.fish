#ifndef BEE_FISH__JSON_H
#define BEE_FISH__JSON_H

#include "../Parser/Parser.hpp"
#include "Version.hpp"
#include "Object.hpp"
#include "Array.hpp"
#include "Number.hpp"
#include "String.hpp"
#include "BlankSpace.hpp"
#include "Constants.hpp"

namespace BeeFishJSON {

   using namespace BeeFishParser;

   inline const auto blankSpace =
      BlankSpace();

   inline const auto undefined =
      Undefined();

   inline const auto _null =
      Null();

   inline const auto _true =
      True();

   inline const auto _false =
      False();

   inline const auto boolean =
      _true or _false;

   inline const auto number =
      Number();

   inline const auto string =
      String();

   inline const auto array =
      Array();

   inline const auto object =
      Object();

   And JSON() {
      return blankSpace and
         (
            undefined or
            _null or
            boolean or
            number or
            string or
            array or
            object
         );
   }

}


#endif