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
      
      size_t _expectedSize {0};
      bool _matchAny {false};
      size_t _read {0};
   public:
      std::string _chars;
      
      using Parser::read;

      UTF8Character() : _matchAny(true)
      {
      }

      UTF8Character(
         const UTF8Character& source
      ) :
         _matchAny(source._matchAny),
         _chars(source._chars)
      {
      }

      UTF8Character(
         wchar_t source
      ) :
         _chars(UTF8Character::fromWChar(source))
      {
      }

      UTF8Character(
         const std::string& bytes
      ) : _chars(bytes)
      {
         
      }

      virtual bool read(char character)
      override
      {

         if (_read == 0)
            setExpectedSize(character);

         bool matched = false;

         if (_matchAny) {
           _chars.push_back(character);
            matched = true;
         }
         else {
            if (_chars[_read] == character)
               matched = true;
            else
               setResult(false);
         }

         ++_read;

         if (matched &&
             _read == _expectedSize)
         {
            UTF8Character utf8(_chars);
            return read(utf8);
         }

         return matched;


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
         out << character._chars;
         return out;
      }

      bool operator == (
         const UTF8Character& rhs
      ) const
      {
         return _chars == rhs._chars;
      }

      bool operator != (
         const UTF8Character& rhs
      ) const
      {
         return _chars != rhs._chars;
      }

      virtual bool read(
         const UTF8Character& character
      ) override
      {
         if (_matchAny) {
            _chars = character._chars;
            setResult(true);
         }
         else if (character._chars == _chars)
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

      }

      virtual bool operator >= (
         const UTF8Character& right
      )
      {

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

      }

      virtual Parser* copy()
      const override
      {
         return new UTF8Character(*this);
      }

   };


}


#endif
