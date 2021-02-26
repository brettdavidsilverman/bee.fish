#ifndef BEE_FISH_PARSER__CIWORD_H
#define BEE_FISH_PARSER__CIWORD_H

#include <stdio.h>
#include <ctype.h>
#include "word.h"

namespace bee::fish::parser {

   using namespace std;

   class CIWord : public Word {
   public:
      CIWord(const string& word) :
         Word(word)
      {
      
      }
      
      CIWord(const BString& word) :
         Word(word)
      {
      
      }
   
      virtual void write(
         ostream& out, 
         size_t tabIndex = 0
      ) const
      {
         writeHeader(out, "CIWord", tabIndex);
         out << "(\"";
         UTF8Character::writeEscaped(out, _word);
         out << "\")";
      }

      CIWord(const CIWord& source) :
         CIWord(source._word)
      {
      }
			   
      virtual Match* copy() const
      {
         return new CIWord(*this);
      }
      
   protected:
      virtual bool match_char(const Char& character) {
   
         if (character == Match::EndOfFile)
            return false;
         
         return (
            tolower(*_index) == 
            tolower(character)
         );
      }
   };
   
   class CIWordPtr : public MatchPtr
   {
   public:
      CIWordPtr(const string& word) :
         MatchPtr(new CIWord(word))
      {
      }
   };

}

#endif
