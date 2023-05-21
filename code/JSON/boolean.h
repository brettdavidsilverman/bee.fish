#ifndef BEE_FISH_JSON__BOOLEAN_H
#define BEE_FISH_JSON__BOOLEAN_H

#include <map>
#include <iomanip>
#include "../misc/optional.h"

#include "../parser/parser.h"

using namespace BeeFishParser;

namespace BeeFishJSON
{

   class Boolean : public Capture
   {
   public:

      Word* _true;
      Word* _false;
      
      Boolean() : Capture(
         new Or (
            _true = new Word("true"),
            _false = new Word("false")
         )
      )
      {
      }
            
   };
}

#endif