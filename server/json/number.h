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
      
      virtual void write(
         ostream& out,
         size_t tabIndex = 0
      ) const
      {
         if (_result != true)
         {
            And::write(out, tabIndex);
            return;
         }
         
         out << tabs(tabIndex);
         
         if (_sign->matched())
            out << _sign->_value;
            
         out << _integer->_value;
         
         if (_fraction->matched())
            out << '.'
                << _fraction->_integer->_value;
                
         if (_exponent->matched())
         {
            _exponent->write(out);
         }
         
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
         
         void write(ostream& out)
         {
            out << 'e'
                << _sign->_value
                << _integer->_value;
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
      
         
   };
}

#endif