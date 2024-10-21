#ifndef BEE_FISH_JSON__UNDEFINED_H
#define BEE_FISH_JSON__UNDEFINED_H

#include "../parser/parser.h"

using namespace BeeFishParser;

namespace BeeFishJSON
{
   class Undefined : public Word {
   public:
      Undefined() : Word("undefined") {

      }
   };
   
}

#endif