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

   And JSON() {
      return BlankSpace() and
         (
            Number() or
            String() or
            Array() or
            Null() or
            True() or
            False() or
            Undefined() or
            Object()
         );
   }

}


#endif