#ifndef BEE_FISH__JSON_H
#define BEE_FISH__JSON_H

#include "../Parser/Parser.hpp"
#include "Version.hpp"
#include "Object.hpp"
#include "Array.hpp"
#include "Number.hpp"
#include "String.hpp"
#include "BlankSpace.hpp"

namespace BeeFishJSON {

   using namespace BeeFishParser;

   inline const auto undefined =
      Word("undefined");

   inline const auto _null =
      Word("null");

   inline const auto _true =
      Word("true");

   inline const auto _false =
      Word("false");

   inline const auto boolean =
      _true or _false;

   enum Type {
      UNDEFINED,
      _NULL,
      BOOLEAN,
      NUMBER,
      STRING,
      ARRAY,
      OBJECT
   };

   And _JSON(Parser* parent) {
      return
         -blankSpace and
         (
            undefined or
            _null or
            boolean or
            number or
            _string or
            array or
            Object()
         );
   }

   Parser* JSON(Parser* params) {
      return _JSON(params).copy();
   }

}


#endif