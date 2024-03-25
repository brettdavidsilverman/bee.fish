#ifndef BEE_FISH_JSON__NUMBER_H
#define BEE_FISH_JSON__NUMBER_H

#include "../parser/parser.h"
#include "integer.h"

namespace BeeFishJSON {
      
   class Number: public Capture
   {
   public:
      

      Match* _fractionInteger;
      
      Match* _fraction = new And(
         new BeeFishParser::Character('.'),
         _fractionInteger = new Integer()
      );
         
      class Plus : public BeeFishParser::Character {
      public:
         Plus() : Character('+') {

         }
      };

      class Minus : public BeeFishParser::Character {
      public:
         Minus() : Character('-') {

         }
      };

      class Sign : public Or {
      public:
         Sign() : Or(
            new Plus(),
            new Minus()
         ) {

         }
      };

      Match* _numberSign = new Minus();
      Match* _exponentSign = new Sign();
      
      Match* _integer = new Integer();
      Match* _exponentInteger = new Integer();
      
      Match* _exponent = new And(
         new Or(
            new BeeFishParser::
               Character('E'),
            new BeeFishParser::
               Character('e')
         ),
         new Optional(_exponentSign),
         _exponentInteger
      );
      
      Match* _number =
         new And(
            new Optional(_numberSign),
            _integer,
            new Optional(_fraction),
            new Optional(_exponent)
         );

      Match* _allValues = new Capture(
         new Or(
            _number,
            new Word("\"NaN\"")
         )
      );

   public:
      Number() : Capture()
      {
         _match = _allValues;
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
      
      virtual BString& value() {
         if (result() == true)
            return _allValues->value();
            
         return Match::value();
      }
      
   };
   
}

#endif