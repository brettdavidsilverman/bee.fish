#ifndef BEE_FISH_JSON__NULL_H
#define BEE_FISH_JSON__NULL_H

#include "../parser/parser.h"

using namespace BeeFishParser;

namespace BeeFishJSON
{
   class Null : public Word {
   public:
      Null() : Word("null") {

      }
   };
   
}

#endif