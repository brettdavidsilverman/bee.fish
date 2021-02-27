#ifndef BEE_FISH_JSON__ARRAY_H
#define BEE_FISH_JSON__ARRAY_H

#include "../parser/parser.h"


using namespace bee::fish::parser;

namespace bee::fish::json {
   
   extern Match* JSON;
   
   class Array :
      public And
   {
      
   public:
      Array() : And(
         new Character('['),
         new Optional(
            new And(
               new LoadOnDemand(JSON),
               new Optional(
                  new Repeat(new Record())
               )
            )
         ),
         new Character(']')
      )
      {
      }
      
      virtual string name()
      {
         return "Array";
      }
      
      class Record : public And
      {
      public:
         Record() : And(
            new Character(','),
            new LoadOnDemand(JSON)
         )
         {
         }
         
         
         
      };

   };
}

#endif