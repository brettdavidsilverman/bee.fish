#ifndef BEE_FISH_JSON__BLANK_SPACE_H
#define BEE_FISH_JSON__BLANK_SPACE_H

#include "../parser/parser.h"

using namespace BeeFishParser;

namespace BeeFishJSON
{
   
   class BlankSpaceCharacter : public Or {
   public:
      BlankSpaceCharacter() : Or(
         new BeeFishParser::
            Character(0x0020),
            
         new BeeFishParser::
            Character(0x000A),
            
         new BeeFishParser::
            Character(0x000D),
            
         new BeeFishParser::
            Character(0x0009),
            
         new BeeFishParser::
            Character('\r'),
            
         new BeeFishParser::
            Character('\n')
      )
      {}
   };
      
   class BlankSpace : public Repeat<BlankSpaceCharacter> {
   public:
      BlankSpace() : Repeat<BlankSpaceCharacter>() {

      }
   };
   
}

#endif