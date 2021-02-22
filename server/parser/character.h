#ifndef BEE_FISH_PARSER__CHARACTER_H
#define BEE_FISH_PARSER__CHARACTER_H

#include "match.h"

namespace bee::fish::parser {

   
   class Character : public Match {
   protected:
      Char _character;
      bool _matchAny;
      Char _matchedCharacter;
      
   public:
   
      Character() :
         Match(),
         _character(-1),
         _matchAny(true),
         _matchedCharacter(-1)
      {
      }
      
      Character(Char character) :
         Match(),
         _character(character),
         _matchAny(false),
         _matchedCharacter(-1)
      {
      }
   
      virtual ~Character()
      {
      }
   
      virtual bool match(const Char& character)
      {
         bool matched;
         
         if (character == Match::EndOfFile)
            matched = false;
         else if (_matchAny)
            matched = true;
         else
            matched =
               (_character == character);
         
         if (matched)
         {
            _matchedCharacter = character;
            Match::match(character);
            success();
         }
         else
         {
            fail();
         }
      
         return matched;
      }
   
      virtual void write(ostream& out) const
      {
         out << "Character";
         writeResult(out);
         out << "(";
         if (!_matchAny)
         {
            out << "'";
            UTF8Character::write(out, character());
            out << "'";
         }
         out << ")";
      }
   
      Character(const Character& source) :
         Match(),
         _character(source._character),
         _matchAny(source._matchAny),
         _matchedCharacter(source._matchedCharacter)
      {
      }
			   
      virtual MatchPtr copy() const
      {
         return MatchPtr(new Character(*this));
      }
      
      virtual const Char& character() const
      {
         return _character;
      }
   

   };


};

#endif
