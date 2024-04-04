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
      
   class Integer : public IntegerCharacters
   {
   public:
      Integer() : IntegerCharacters()
      {
      }
      
      virtual void eof(Parser* parser) 
      override {

          if (!_parser)
             setup(parser);
            
          _parser = parser;

          if (result() != nullopt)
             return;
             
          if (_matchedCount > 0)
          {
              success();
              //cerr << "Integer::eof::success::" << result() << endl;
          }
          else
          {
              fail();
              //cerr << "Integer::eof::fail::" << result() << endl;
          }
          
      }
      
   };
   
}

#endif