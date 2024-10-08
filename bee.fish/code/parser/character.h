#ifndef BEE_FISH_PARSER__CHARACTER_H
#define BEE_FISH_PARSER__CHARACTER_H

#include "match.h"

namespace BeeFishParser {

   class Character : public Match {
   protected:
      Char _characterToMatch;
      bool _any;
      
   public:
      Character() :
         Match(),
         _characterToMatch(""),
         _any(true)
      {
      }
      
      Character(const Char& character) :
         Match(),
         _characterToMatch(character),
         _any(false)
      {
      }
      
      virtual bool matchCharacter(const Char& character)
      {
         bool matched =
            ( _any ||
              ( _characterToMatch == character )
            );
         
         if (matched)
         {
            success();
         }
         else
         {
            fail();
         }
      
         return matched;
      }
   };


}


#endif
