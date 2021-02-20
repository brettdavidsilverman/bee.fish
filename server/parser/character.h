#ifndef BEE_FISH_PARSER__CHARACTER_H
#define BEE_FISH_PARSER__CHARACTER_H

#include "match.h"

namespace bee::fish::parser {

   
   class Character : public Match {
   protected:
      WideChar _character;
      bool _matchAny;
      WideChar _matchedCharacter;
      
   public:
   
      Character() :
         Match(),
         _character(-1),
         _matchAny(true),
         _matchedCharacter(-1)
      {
      }
      
      Character(WideChar character) :
         Match(),
         _character(character),
         _matchAny(false),
         _matchedCharacter(-1)
      {
      }
   
      virtual ~Character()
      {
      }
   
      virtual bool match(WideChar character)
      {
         bool matched;
         
         if (_matchAny)
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
   
      virtual void write(ostream& out)
      {
         out << "Character";
         writeResult(out);
         out << "(";
         if (!_matchAny)
         {
            out << "'";
            Match::write(out, _character);
            out << "'";
         }
         out << ")";
      }
   
      Character(const Character& source) :
         Match(source),
         _character(source._character),
         _matchAny(source._matchAny),
         _matchedCharacter(source._matchedCharacter)
      {
      }
			   
      virtual Match* copy() const
      {
         return new Character(*this);
      }
      
      virtual WideChar& character()
      {
         return _matchedCharacter;
      }
   

   };


};

#endif
