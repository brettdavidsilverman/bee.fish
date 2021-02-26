#ifndef BEE_FISH_PARSER__CHARACTER_H
#define BEE_FISH_PARSER__CHARACTER_H

#include "match.h"

namespace bee::fish::parser {

   class Character : public Match {
   protected:
      Char _character;
      bool _anyCharacter;
      
   public:
      Character() :
         _character(-1),
         _anyCharacter(true)
      {
      }
      
      Character(const Char& character) :
         _character(character),
         _anyCharacter(false)
      {
      }
      
      Character(const Character& source) :
         _character(source._character),
         _anyCharacter(source._anyCharacter)
      {
         _character = source._character;
      }
			   
      virtual ~Character()
      {
      }
   
      virtual bool match(const Char& character)
      {
         bool matched =
            (character != Match::EndOfFile) &&
            (
               _anyCharacter ||
               (_character == character)
            );
         
         if (matched)
         {
            Match::match(character);
            success();
         }
         else
         {
            fail();
         }
      
         return matched;
      }
   
      virtual string name()
      {
         ostringstream out;
         out << "\"Char";
         Match::write(out, _character);
         out << "\"";
         return out.str();
      }
   
      
      virtual MatchPtr copy() const
      {
         return MatchPtr(new Character(*this));
      }
   

   };

   class CharacterPtr : public MatchPtr
   {
   public:
      CharacterPtr(const Char& character) :
         MatchPtr(new Character(character))
      {
      }
      
   };
};

#endif
