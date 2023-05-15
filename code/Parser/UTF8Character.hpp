#ifndef BEE_FISH_PARSER__UTF8_CHARACTER_HPP
#define BEE_FISH_PARSER__UTF8_CHARACTER_HPP
#include <string>
#include <vector>
#include "Char.hpp"
#include "Parser.hpp"

namespace BeeFishParser {

   class UTF8Character :
      public Parser
   {
   protected:
      Char _currentChar;
      size_t _expectedCharCount {0};
      std::string _chars;
      bool _matchAny {false};
   public:

      using Parser::read;

      UTF8Character() : _matchAny(true)
      {
      }

      UTF8Character(
         const std::string& bytes
      ) :
         _chars(bytes)
      {
      }

      virtual bool read(bool bit)
      override
      {

         //std::cerr << (bit ? 1 : 0) << std::flush;

         Char& c = _currentChar;
       
         if (c.read(bit)) {
            if (c._result == true) {

               if (_currentChars.length() == 0) {
                  _expectedCharCount =
                     getExpectedCharCount();
               }

               _currentChars.push_back(c._char);
               _currentChar = Char();

               if (_expectedCharCount ==
                   _currentChars.length())
               {
                  UTF8Character character(_currentChars);
                  _currentChars.clear();
                  return read(character);
               }
               return true;
            }
            return true;
         }
         
         return false;
      }
      
      size_t getExpectedCharCount() {
         Char& first = _currentChar;
         size_t count;
         for (count = 1;
              first._bits[8 - count];
              ++count)
         {
         }

         return count;
      }

      std::wstring toWString() {

         std::wstring_convert<
            std::codecvt_utf8<wchar_t>
         > utf8Convert;

         std::wstring wstring =
            utf8Convert
               .from_bytes(_chars);
         
         return wstring;

      }

      static UTF8Character fromWString(
         const std::wstring& wstring
      )
      {
         std::wstring_convert<
            std::codecvt_utf8<wchar_t>
         > utf8Convert;

         std::string bytes =
            utf8Convert.to_bytes(wstring);
         
         return UTF8Character(bytes);
      }

      friend std::ostream& operator << (
         std::ostream& out,
         const UTF8Character& character
      )
      {
         out << character._chars;
         return out;
      }

      bool operator == (
         const UTF8Character& rhs
      ) const
      {
         return _chars == rhs._chars;
      }

      virtual bool read(
         const UTF8Character& character
      ) override
      {
         if (_matchAny) {
            _chars = character._chars;
            setResult(true);
         }
         else if (character == *this)
            setResult(true);
         return true;
      }
   };

   typedef UTF8Character Character;

}


#endif
