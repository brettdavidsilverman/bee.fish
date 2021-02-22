#ifndef BEE_FISH_PARSER__CIWORD_H
#define BEE_FISH_PARSER__CIWORD_H

#include <stdio.h>
#include <ctype.h>
#include "word.h"

namespace bee::fish::parser {

   using namespace std;

   class CIWord : public Word {
   public:
      CIWord(const BString& word) :
         Word(word)
      {
      
      }
   
      CIWord(const string& word) :
         CIWord(BString(word))
      {
      }

      virtual void write(ostream& out) const
      {
         out << "CIWord";
         writeResult(out);
         out << "(\"" << _word << "\")";
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

}

#endif
