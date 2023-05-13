#ifndef BEE_FISH_PARSER__UTF8_CHARACTER_HPP
#define BEE_FISH_PARSER__UTF8_CHARACTER_HPP
#include <string>
#include <vector>
#include "Parser.hpp"

namespace BeeFishParser {

   class Bit : public Parser {
   public:
      bool _bit;

      virtual bool read(bool bit)
      override
      {
         _bit = bit;
         setResult(true);
         return true;
      }
   };


   class Char : public Parser
   {
   public:
      std::bitset<8> _bits;
      int _bitCount {0};
      char _char {-1};

      Char()
      {
      }

      virtual bool
      read(bool bit) override
      {

         _bits[7 - _bitCount++] = bit;
         if (_bitCount == 8) {
            setResult(true);
         }
         return true;
      }


      virtual void setResult(
         Optional result
      ) override
      {
         if (result == true) {
            _char = _bits.to_ulong();
         }

         Parser::setResult(result);
      }

   };

   class UTF8Character :
      public Parser
   {

   private:
      bool _first {true};
      Char _current;
      size_t _expectedCharCount {0};
      std::string _chars;
   public:

      using Parser::read;

      UTF8Character()
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

        // std::cerr << (bit ? 1 : 0) << std::flush;

         Char& c = _current;
       
         if (c.read(bit)) {
            if (c._result == true) {
               if (_first) {
                  _chars.clear();
                  _first = false;
               }
               _chars.push_back(c._char);
               if (_chars.size() == 1) {
                  _expectedCharCount =
                     getExpectedCharCount();
               }
               _current = Char();
               if (_expectedCharCount ==
                      _chars.size())
               {
                  return read(*this);
               }
               return true;
            }
            return true;
         }
         
         return false;
      }

      size_t getExpectedCharCount() {
         Char& first = _current;
         size_t count;
         for (count = 0;
              first._bits[7 - count];
              ++count)
         {
         }

         return count;
      }

      virtual bool read(
         const UTF8Character& character
      ) override
      {
         return true;
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
      ) 
      {
         return _chars == rhs._chars;
      }

   
   };


}


#endif
