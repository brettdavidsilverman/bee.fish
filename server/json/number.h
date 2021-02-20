#ifndef BEE_FISH_PARSER_JSON__NUMBER_H
#define BEE_FISH_PARSER_JSON__NUMBER_H

#include "../parser.h"

namespace bee::fish::parser::json {
      
   class Number :
      public And,
      public Value
   {
   public:
      Number() : And(
         new Optional(
            new Sign()
         ),
         new Integer(),
         new Optional(
            new Fraction()
         ),
         new Optional(
            new Exponent()
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
      
      class Integer : public
         Repeat<IntegerCharacter>
      {
      public:
         Integer() : Repeat()
         {
         }

      };
      
      class Fraction : public And
      {
      public:
         Fraction() : And(
            new Character('.'),
            new Integer()
         )
         {
         }
      };
      
      class Exponent : public And
      {
      public:
         Exponent() : And(
            new Or(
               new Character('E'),
               new Character('e')
            ),
            new Sign(),
            new Integer()
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
         }
      };
      
      virtual void write(ostream& out)
      {
         out << value();
      }
         
   };
}

#endif