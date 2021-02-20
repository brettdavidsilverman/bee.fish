#ifndef BEE_FISH_PARSER__CIWORD_H
#define BEE_FISH_PARSER__CIWORD_H

#include <stdio.h>
#include <ctype.h>
#include "word.h"

namespace bee::fish::parser {

   using namespace std;

   class CIWord : public Word {
   public:
      CIWord(const wstring& word) :
         Word(word)
      {
      
      }
   
   
      virtual void write(ostream& out)
      {
         out << "CIWord";
         writeResult(out);
         out << "(\"";
         Match::write(out, _word);
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
      virtual bool match_char(WideChar character) {
   
         if (character == Match::EndOfFile)
            return false;
         
         return (
            tolower(*_index) == 
            tolower(character)
         );
      }
   };

}

#endif
