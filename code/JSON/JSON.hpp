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

   const And JSON() {
      return BlankSpace() and
         (
            Undefined() or
            Null() or
            True() or
            False() or
            Number() or
            String() or
            Array() or
            Object()
         );
   }

}


#endif