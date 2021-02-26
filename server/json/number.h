#ifndef BEE_FISH_JSON__NUMBER_H
#define BEE_FISH_JSON__NUMBER_H

#include "../parser/parser.h"

namespace bee::fish::json {
      
   class Number : public And
   {
   public:
      class Sign;
      class Integer;
      class Fraction;
      class Exponent;
      
      Sign* _sign;
      Integer* _integer;
      Fraction* _fraction;
      Exponent* _exponent;
      
   public:
      Number() : And(
         new Optional(
            _sign = new Sign()
         ),
         _integer = new Integer(),
         new Optional(
            _fraction = new Fraction()
         ),
         new Optional(
            _exponent = new Exponent()
         )
      )
      {
      }
      
      class IntegerCharacter : public Range
      {
      public:
         IntegerCharacter() : Range('0', '9')
         {
         }
            
      };
      
      class Integer : public Repeat
      {
      public:
         Integer() : Repeat(
            new IntegerCharacter()
         )
         {
            _capture = true;
         }

      };
      
      class Fraction : public And
      {
      public:
         Integer* _integer;
         
      public:
         Fraction() : And(
            new Character('.'),
            _integer = new Integer()
         )
         {
         }
      };
      
      class Exponent : public And
      {
      public:
         Sign* _sign;
         Integer* _integer;
         
      public:
         Exponent() : And(
            new Or(
               new Character('E'),
               new Character('e')
            ),
            new Optional(
               _sign = new Sign()
            ),
            _integer = new Integer()
         )
         {
         }
      };
      
      class Sign : public Or
      {
      public:
         Sign() : Or(
            new Character('+'),
            new Character('-')
         )
         {
            _capture = true;
         }
      };
      
      virtual void write(ostream& out)
      {
         out << "Number";
      }
         
   };
}

#endif