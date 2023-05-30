#ifndef BEE_FISH_PARSER__UTF8_CHARACTER_HPP
#define BEE_FISH_PARSER__UTF8_CHARACTER_HPP
#include <string>
#include <vector>
#include <locale>
#include <codecvt>
#include "Char.hpp"
#include "ParserBase.hpp"
#include "Word.hpp"

namespace BeeFishParser {

   class UTF8Character :
      public Parser
   {
   protected:
      std::string _chars;
      size_t _expectedSize {0};
      bool _matchAny {false};
      
   public:
      wchar_t _character {0};
      
      using Parser::read;

      UTF8Character() : _matchAny(true)
      {
      }

      UTF8Character(
         const UTF8Character& source
      ) :
         _matchAny(source._matchAny),
         _character(source._character)
      {
      }

      UTF8Character(
         wchar_t source
      ) :
         _character(source)
      {
      }

      UTF8Character(
         const std::string& bytes
      ) : _character(toWChar(bytes))
      {
         
      }
/*
      virtual bool read(bool bit)
      override
      {

         Char& c = _char;
       
         if (c.read(bit)) {

            if (c._result == true) {

               if (_chars.length() == 0) {
                  _expectedSize =
                     getExpectedCharCount();
               }

               _chars.push_back(c._char);
               c = Char();
               if (_expectedSize ==
                   _chars.length())
               {
                 UTF8Character character(_chars);
                  _chars.clear();
                  return read(character);
               }
               return true;
            }
            return true;
         }
         
         return false;
      }
*/

      virtual bool read(char character)
      override
      {

         if (_chars.length() == 0)
            setExpectedSize(character);

         _chars.push_back(character);


         if (_chars.length() >=
             _expectedSize)
         {
            UTF8Character utf8(_chars);
            _chars.clear();
            _expectedSize = 0;
            return read(utf8);
         }
         
         return true;

      }

      void setExpectedSize(char first) {
         
         std::bitset<8> bits = first;

         size_t count {0};

         while (count < 8 &&
                bits[7 - count])
              ++count;

         if (count == 0)
            count = 1;

         _expectedSize = count;

      }
/*
      size_t getExpectedCharCount() {
         Char& first = _char;

         size_t count {0};

         while (count < 8 &&
                first._bits[7 - count])
              ++count;

         if (count == 0)
            count = 1;

         return count;
      }
*/
      static wchar_t toWChar(const std::string& bytes) {

         static std::wstring_convert<
            std::codecvt_utf8<wchar_t>
         > utf8Convert {};

         std::wstring wstring =
            utf8Convert
               .from_bytes(bytes);
         
         return wstring[0];

      }

      static std::string fromWChar(
         wchar_t character
      )
      {

         static std::wstring_convert<
            std::codecvt_utf8<wchar_t>
         > utf8Convert {};

         std::wstring wstring;
         wstring.push_back(character);

         std::string bytes =
            utf8Convert.to_bytes(wstring);
         
         return bytes;
      }

      friend std::ostream& operator << (
         std::ostream& out,
         const UTF8Character& character
      )
      {
         out << fromWChar(character._character);
         return out;
      }

      bool operator == (
         const UTF8Character& rhs
      ) const
      {
         return _character == rhs._character;
      }

      bool operator != (
         const UTF8Character& rhs
      ) const
      {
         return _character != rhs._character;
      }

      virtual bool read(
         const UTF8Character& character
      ) override
      {
         if (_matchAny) {
            _character = character._character;
            setResult(true);
         }
         else if (character._character == _character)
            setResult(true);
         else {
            setResult(false);
            return false;
         }
         return true;
      }

      virtual bool operator <= (
         const UTF8Character& right
      )
      {
         return _character <= right._character;
/*
         if (_chars.length() < right._chars.length())
            return true;

         if (_chars.length() > right._chars.length())
            return false;

         for (size_t i = 0;
              i < _chars.length();
              ++i)
         {
            if (_chars[i] < right._chars[i])
               return true;
            else if (_chars[i] > right._chars[i])
               return false;
         }
         return true;
*/
      }

      virtual bool operator >= (
         const UTF8Character& right
      )
      {
         return _character >= right._character;

/*
         if (_chars.length() > right._chars.length())
            return true;

         if (_chars.length() < right._chars.length())
            return false;

         for (size_t i = 0;
              i < _chars.length();
              ++i)
         {
            if (_chars[i] > right._chars[i])
               return true;
            else if (_chars[i] < right._chars[i])
               return false;
         }
         return true;
*/
      }

      virtual Parser* copy()
      const override
      {
         return new UTF8Character(*this);
      }

   };


}


#endif
