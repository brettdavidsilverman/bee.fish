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
         _characterToMatch(-1),
         _any(true)
      {
         _setup = true;
      }
      
      Character(const Char& character) :
         Match(),
         _characterToMatch(character),
         _any(false)
      {
         _setup = true;
      }
      
      virtual bool matchCharacter(const Char& character)
      {
         bool matched =
            ( _any ||
              ( _characterToMatch == character )
            );
         
         if (matched)
         {
            _result = true;
         }
         else
         {
            _result = false;
         }
      
         return matched;
      }
   };


}


#endif
