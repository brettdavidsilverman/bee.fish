#ifndef BEE_FISH_PARSER__CHARACTER_H
#define BEE_FISH_PARSER__CHARACTER_H

#include "UTF8Character.hpp"

namespace BeeFishParser {

   class Character :
      public UTF8Character
   {
   public:
      using UTF8Character::read;

      Character()
      {
      }

      Character(const UTF8Character& character) :
         UTF8Character(character)
      {
      }

      virtual Optional read(
         const UTF8Character& character
      ) override
      {
 
         bool matched =
            ( *this == character );
         
         if (matched)
         {
            setResult(true);
         }
         else
         {
            setResult(false);
         }
      
         return matched;
      }

      

   };


}


#endif
