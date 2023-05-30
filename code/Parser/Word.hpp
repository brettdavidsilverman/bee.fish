#ifndef BEE_FISH_PARSER__WORD_HPP
#define BEE_FISH_PARSER__WORD_HPP
#include <sstream>
#include <string>

#include "Character.hpp"

namespace BeeFishParser {

   class Word :
      public Parser
   {
   protected:
      std::string _word;
      size_t _index {0};

   public:
      using Parser::read;

      Word(const Word& source) :
         _word(source._word)
      {
      }

      Word(const std::string& word) :
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

      bool operator == (const std::string& rhs) {
         return _word == rhs;
      }

      friend std::ostream& operator <<
      (std::ostream& out, const Word& word)
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
