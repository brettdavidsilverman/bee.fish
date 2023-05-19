#ifndef BEE_FISH_PARSER__WORD_HPP
#define BEE_FISH_PARSER__WORD_HPP
#include <sstream>

#include "Character.hpp"

namespace BeeFishParser {

   class Word :
      public std::string,
      public Parser
   {
   protected:
      size_t _index {0};

   public:
      using Parser::read;

      Word(const Word& source) :
         std::string(source)
      {
      }

      Word(const std::string& source) :
         std::string(source)
      {
      }

      virtual bool read(char c) override {

         if (_index >= size())
            return false;

         if ((*this)[_index] == c)
         {
            if (++_index == size())
               setResult(true);

            return true;
         }
         
         setResult(false);
         return false;

      }

      virtual bool read(bool bit) override {
         throw std::logic_error("Should not reach here");
      }

      virtual Parser* copy() const {
         return new Word(*this);
      }

      bool operator == (const std::string& rhs) {
         return ::operator == (*this, rhs);
      }

      
   
   };


};

#endif
