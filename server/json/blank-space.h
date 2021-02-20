#ifndef BEE_FISH_PARSER__BLANK_SPACE_H
#define BEE_FISH_PARSER__BLANK_SPACE_H

#include "../parser.h"

using namespace bee::fish::parser;

namespace bee::fish::parser::json
{
   
   class BlankSpaceCharacter : public Or
   {
   public:
      BlankSpaceCharacter() : Or (
         new Character(0x0020),
         new Character(0x000A),
         new Character(0x000D),
         new Character(0x0009),
         new Character('\r'),
         new Character('\n')
      )
      {
      }
   };
      
   class BlankSpace : public Repeat<BlankSpaceCharacter>
   {
   public:
      BlankSpace() : Repeat()
      {
      }
   };

   
   
}

#endif