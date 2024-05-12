#ifndef BEE_FISH_PARSER__WORD
#define BEE_FISH_PARSER__WORD

#include "match.h"
#include "../b-string/b-string.h"
#include "../Size.hpp"

namespace BeeFishParser {

   using namespace std;
            
   class Word : public Match {
   protected:
   
      BString _word;
      BString::const_iterator _index;
   
      virtual bool matchChar(const Char& character)
      {
         Size countMatches = 0;
         for (auto c : character)
         {
            if (c != (*_index))
               return false;
               
            countMatches++;
            
            if (++_index == _word.cend())
               break;
         }
         
         return countMatches == character.size();
      }
   
   public:

      Word(BString word) :
         Match(),
         _word(word)
      {
         _index = _word.cbegin();
         _result = nullopt;
      }
     
      virtual bool matchCharacter(const Char& character)
      {
         
         bool matched = matchChar(character);
         
         if (matched)
         {
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
