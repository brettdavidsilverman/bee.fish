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

   inline const static auto blankSpace =
      BlankSpace();

   inline const static auto undefined =
      Undefined();

   inline const static auto _null =
      Null();

   inline const static auto _true =
      True();

   inline const static auto _false =
      False();

   inline const static auto boolean =
      _true or _false;

   inline const static auto number =
      Number();

   inline const static auto string =
      String();

   inline const static auto array =
      Array();

   inline const static auto object =
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