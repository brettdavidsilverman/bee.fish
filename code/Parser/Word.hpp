#ifndef BEE_FISH_PARSER__WORD_HPP
#define BEE_FISH_PARSER__WORD_HPP
#include <sstream>
#include <string>

#include "Character.hpp"

namespace BeeFishParser {

   using namespace std;

   class Word :
      public Parser
   {
   protected:
      string _word;
      size_t _index {0};

   public:
      using Parser::read;

      Word(const Word& source) :
         _word(source._word)
      {
      }

      Word(const string& word) :
         _word(word)
      {
      }

      virtual bool read(char c)
      override
      {

         if (_index >= _word.size()) {
            return false;
         }

         if (_word[_index++] == c)
         {

            if (_index == _word.size()) {
               setResult(true);
            }

            return true;
         }
 
         setResult(false);
         return false;

      }

      virtual Parser* copy() const {
         return new Word(*this);
      }

      bool operator == (const string& rhs) {
         return _word == rhs;
      }

      friend ostream& operator <<
      (ostream& out, const Word& word)
      {
         out << word._word;
         return out;
      }

      virtual size_t index() override {
         return _index;
      }
      
      
   
   };


};

#endif
