#ifndef BEE_FISH_JSON__ARRAY_H
#define BEE_FISH_JSON__ARRAY_H

#include "../power-encoding/power-encoding.h"
#include "../parser/parser.h"
#include "blank-space.h"
#include "number.h"
#include "set.h"

using namespace BeeFishParser;
using namespace BeeFishPowerEncoding;

namespace BeeFishJSON {
   
   class JSON;
   class JSONParser;
  
   class ArrayOpenBrace : public And {
   public:
      ArrayOpenBrace() : And(
         new Character('['),
         new BlankSpace(0)
      )
      {

      }
   };

   class ArrayCloseBrace : public And {
   public:
      ArrayCloseBrace() : And(
         new BlankSpace(0),
         new Character(']')
      )
      {

      }
   };

   class ArraySeperator : public And {
   public:
      ArraySeperator() : And(
         new BlankSpace(0),
         new Character(','),
         new BlankSpace(0)
      )
      {

      }
   };
   
   typedef LoadOnDemand<JSON> ArrayValue;
   
   class Array : public Set<ArrayOpenBrace, ArrayValue, ArraySeperator, ArrayCloseBrace>
   {
   public:
      size_t _size = 0;

   public:
      Array() : Set()
      {
      }
      
      
      virtual ~Array()
      {
      }
      
      // Defined in json-parser.h
      virtual void onbeginset(Match* match);

      // Defined in json-parser.h
      virtual void onarrayvalue(JSON* match);
      
      // Defined in json-parser.h
      virtual void onendset(Match* match);

      BeeFishJSON::JSONParser* jsonParser() {
         return (BeeFishJSON::JSONParser*)_parser;
      }
      
      virtual void onsetvalue(ArrayValue* match)
      override
      {
         JSON* json = (JSON*)(match->_match);
         assert(json);
         onarrayvalue(json);
      }
      /*
      virtual ArrayValue* createItem() {
         ArrayValue* arrayValue = new ArrayValue();
         if (_parser)
            arrayValue->setup(_parser);
         return arrayValue;
      }
      */

   };
    
}

#endif