#ifndef BEE_FISH_PARSER__WORD
#define BEE_FISH_PARSER__WORD
#include <string.h>
#include "match.h"


namespace bee::fish::parser {

using namespace std;
            
   class Word : public Match {
   protected:
   
      
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
      BString _word;
      
      Word(const string& word) :
         _word(BString(word))
      {
         _index = _word.cbegin();
      }
      
      Word(const BString& word) :
         _word(word)
      {
         _index = _word.cbegin();
      }
   
      virtual bool match(const Char& character)
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
   
      virtual void write(ostream& out, size_t tabIndex = 0) const
      {
         writeHeader(out, "Word", tabIndex);
         out << "(\"";
         UTF8Character::writeEscaped(out, _word);
         out << "\")";
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

   class WordPtr : public MatchPtr
   {
   public:
      WordPtr(const string& word) :
         MatchPtr(new Word(word))
      {
      }
   };

};

#endif
