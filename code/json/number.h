#ifndef BEE_FISH_JSON__NUMBER_H
#define BEE_FISH_JSON__NUMBER_H

#include "../parser/parser.h"
#include "integer.h"

namespace BeeFishJSON {
      
      
      
      
   class Number: public Match
   {
   public:
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
       
      Match* _numberSign      = new Minus();
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
         
      Match* _number =
         new And(
            new Optional(_numberSign),
            _integer,
            new Optional(_fraction),
            new Optional(_exponent)
         );
         
   public:
      Number() : Match()
      {
         _match = new Capture(_number);
      }
      
      virtual void eof(Parser* parser)
      {
         setup(parser);
         
         if (result() != nullopt)
            return;
            
         if (_integer->result() == nullopt)
            _integer->eof(parser);
         
         if (_integer->result() == true)
         {
            success();
            
         }
         Match::eof(parser);
         
      }
      
      
      
   };
   
}

#endif