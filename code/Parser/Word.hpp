#ifndef BEE_FISH_PARSER__WORD_HPP
#define BEE_FISH_PARSER__WORD_HPP
#include <sstream>
#include "Parser.hpp"
#include "Character.hpp"

namespace BeeFishParser {

            
   class Word :
      public std::vector<Character>,
      public Character
   {
   protected:
      size_t _index {0};

   public:
      using Character::read;

      Word(const Word& source) :
         std::vector<Character>(source)
      {
      }

      Word(const std::string& source) {
         Character character;
         for (const char& c : source)
         {
            if (!character.read(c))
               throw std::runtime_error("Invalid read character");

            if (character._result == true) {
               std::vector<Character>::push_back(character);
               character = Character();
            }
            else if (character._result == false) {
               throw std::runtime_error("Invalid character");
            }

         }
      }

      
      friend ostream& operator << (ostream& out, const Word& word) {
         for (const Character& character : word) {
            out << character._chars;
         }
         return out;
      }

      virtual bool read(const Character& character) {
         
         if (_index >= size())
            return false;

         bool matched = (character == (*this)[_index]);
         
         if (matched)
         {
            ++_index;
            if (_index == size())
            {
               setResult(true);
            }
         
         }
         else
         {
            setResult(false);
         }
       
         return matched;
      }
   
      virtual Parser* copy() const {
         return new Word(*this);
      }

      bool operator == (const std::string& rhs) {
         Word word(rhs);
         return *this == word;
      }

      bool operator == (const Word& rhs) {
         if (size() != rhs.size())
            return false;

         for (size_t i = 0;
              i < size();
              ++i)
         {
            if ((*this)[i] != rhs[i])
               return false;
         }
         
         return true;
      }
   
   };


};

#endif
