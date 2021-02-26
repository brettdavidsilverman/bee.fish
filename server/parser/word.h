#ifndef BEE_FISH_PARSER__WORD
#define BEE_FISH_PARSER__WORD

#include "match.h"
#include <string.h>

namespace bee::fish::parser {

using namespace std;
            
   class Word : public Match {
   protected:
   
      string _word;
      string::const_iterator _index;
   
      virtual bool match_char(const Char& character)
      {
         if (character == Match::EndOfFile)
            return false;
         return
            (
               (*_index) == (char)character
            );
      }
   
   public:

      Word(const string& word) :
         Match(),
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
   
      virtual void write(ostream& out)
      {
         out << "Word(";
         Match::write(out);
         out << ":\"" << _word << "\"";
         out << ")";
      }
   
      virtual const string& word()
      {
         return _word;
      }
   
      Word(const Word& source) :
         Word(source._word)
      {
      }
			   
      virtual MatchPtr copy() const
      {
         return MatchPtr(new Word(*this));
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
