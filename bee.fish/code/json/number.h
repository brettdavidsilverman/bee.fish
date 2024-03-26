#ifndef BEE_FISH_JSON__NUMBER_H
#define BEE_FISH_JSON__NUMBER_H

#include "../parser/parser.h"
#include "integer.h"

namespace BeeFishJSON {
      
      class Plus : public BeeFishParser::Character {
      public:
         Plus() : Character('+') {
          
         }
      };

      class Minus : public BeeFishParser::Character {
      public:
         Minus() : Character('-') {
            assert(this);
         }
      };

      class Sign : public Capture {
      public:
         Sign() : Capture(
            new Or(
               new Plus(),
               new Minus()
            )
         )
         {

         }
      };

      
      
   class Number: public Match
   {
   public:
      BString _sign;
   public:
      Number() : Match()
      {
      Match* numberSign      = new Capture(new Minus(), _sign);
      Match* _integer         = new Integer();
      Match* _fractionInteger = new Integer();
      Match* _fraction        =
         new And(
            new BeeFishParser::Character('.'),
            _fractionInteger
         );
      Match* _exponentSign    = new Sign();
      Match* _exponentInteger = new Integer();
      Match* _exponent        = 
         new And(
            new Or(
               new BeeFishParser::
                  Character('E'),
               new BeeFishParser::
                  Character('e')
            ),
            new Optional(_exponentSign),
            _exponentInteger
         );
         
      Match* _number          =
         new And(
            new Optional(numberSign),
            _integer,
            new Optional(_fraction),
            new Optional(_exponent)
         );
         _match = _number;
      }
      
      virtual void eof(Parser* parser) 
      override {
          
          if (!_setup)
             setup(parser);
             
          if (_result == nullopt)
          {
             _parser->read((char)-1);
          }
          
      }
      
      
   };
   
}

#endif