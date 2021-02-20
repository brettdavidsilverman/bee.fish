#ifndef BEE_FISH_PARSER__WORD
#define BEE_FISH_PARSER__WORD

#include "match.h"
#include <string.h>

namespace bee::fish::parser {

using namespace std;
            
   class Word : public Match {
   protected:
   
      wstring _word;
      wstring::const_iterator _index;
   
      virtual bool match_char(WideChar character)
      {
         if (character == Match::EndOfFile)
            return false;
         return
            (
               (*_index) == character
            );
      }
   
   public:

      Word(const wstring& word) :
         Match(),
         _word(word)
      {
         _index = _word.cbegin();
      }
   
      virtual bool match(WideChar character)
      {
   
         bool matched = match_char(character);

         if (matched)
         {
            Match::match(character);
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
   
      virtual void write(ostream& out)
      {
         out << "Word";
         writeResult(out);
         out << "(\"";
         Match::write(out, _word);
         out << "\")";
      }
   
      virtual const wstring& word()
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
