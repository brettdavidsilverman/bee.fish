#ifndef BEE_FISH_PARSER__WORD
#define BEE_FISH_PARSER__WORD
#include <string.h>
#include "match.h"

namespace bee::fish::parser {

using namespace std;
            
   class Word : public Match {
   protected:
   
      BString _word;
      BString::const_iterator _index;
   
      virtual bool match_char(const Char& character)
      {
         if (character == Match::EndOfFile)
            return false;
         return
            (
               (*_index) == character
            );
      }
   
   public:

      Word(const BString& word) :
         Match(),
         _word(word)
      {
         _index = _word.cbegin();
      }
      
      Word(const string& word) :
         Word(BString(word))
      {
      }
   
      virtual bool match(const Char& character)
      {
   
         bool matched = match_char(character);

         if (matched)
         {
            ++_index;
            if (_index == _word.end())
            {
               success();
            }
         
         }
         else
         {
            fail();
         }
       
         return matched;
      }
   
      virtual void write(ostream& out) const
      {
         out << "Word";
         writeResult(out);
         out << "(\"" << word() << "\")";
      }
   
      virtual const BString& word() const
      {
         return _word;
      }
   
      Word(const Word& source) :
         Word(source._word)
      {
      }
			   
      virtual Match* copy() const
      {
         return new Word(*this);
      }
   
   };


};

#endif
