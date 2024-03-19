#ifndef BEE_FISH_PARSER__WORD
#define BEE_FISH_PARSER__WORD

#include "match.h"
#include "../b-string/string.h"

namespace BeeFishParser {

   using namespace std;
            
   class Word : public Match {
   protected:
   
      BString _word;
      BString::const_iterator _index;
   
      virtual bool matchChar(const Char& character)
      {
         return
            (
               (*_index) == character
            );
      }
   
   public:

      Word(BString word) :
         Match(),
         _word(word)
      {
         _index = _word.cbegin();
      }
     
      virtual bool matchCharacter(const Char& character)
      {
         
         bool matched = matchChar(character);
         
         if (matched)
         {
            ++_index;
            if (_index == _word.end())
            {
               _result = true;
            }
         
         }
         else
         {
            _result = false;
         }
       
         return matched;
      }
   
      const BString& word() const
      {
         return _word;
      }
   
   };


};

#endif
