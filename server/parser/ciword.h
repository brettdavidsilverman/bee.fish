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
   
   
      virtual void write(ostream& out) {
         out << "CIWord(";
         Match::write(out);
         out << ":\"" << _word << "\""
             << ")"
             << endl;
      }

      CIWord(const CIWord& source) :
         CIWord(source._word)
      {
      }
			   
      virtual MatchPtr copy() const
      {
         return MatchPtr(new CIWord(*this));
      }
      
   protected:
      virtual bool match_char(const Char& character) {
   
         if (character == Match::EndOfFile)
            return false;
         
         return (
            tolower(*_index) == 
            tolower((char)character)
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
