#ifndef BEE_FISH_JSON__INTEGER_H
#define BEE_FISH_JSON__INTEGER_H

#include "../parser/parser.h"

namespace BeeFishJSON {
      
   class IntegerCharacter :
      public Range
   {
   public:
      IntegerCharacter() : Range('0', '9') {

      }
   };
      
   class IntegerCharacters : public Repeat<IntegerCharacter> {
   public:
      IntegerCharacters() : Repeat<IntegerCharacter>(1) {

      }
   };
      
   class Integer: public Capture
   {
   public:
      

   public:
      Integer() : Capture(
         new IntegerCharacters()
      )
      {
      }
      
      virtual void eof(Parser* parser) 
      override {
          if (!_setup)
             setup(parser);
             
          if (result(nullopt))
          {
             _parser->read((char)-1);
          }
          
      }
      
   };
   
}

#endif