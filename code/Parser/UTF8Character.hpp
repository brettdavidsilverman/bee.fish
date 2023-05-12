#ifndef BEE_FISH_PARSER__UTF8_CHARACTER_HPP
#define BEE_FISH_PARSER__UTF8_CHARACTER_HPP
#include <string>
#include <vector>
#include "Parser.hpp"

namespace BeeFishParser {

   class Bit : public Parser {
   public:
      bool _bit;

      virtual Optional read(bool bit) {
         _bit = bit;
         setResult(true);
         return true;
      }
   };

   class Char : public Parser
   {
   public:
      std::vector<Bit> _bits;
      int _bitCount {0};

      Char() :
         _bits(8)
      {
      }

      virtual Optional
      read(bool bit)
      {

         if (_bits[_bitCount].read(bit) == true &&
             _bits[_bitCount]._result == true) {
            if (++_bitCount == 8) {
               setResult(true);
            }
            return true;
         }
         else {
            setResult(false);
         }
         return false;
      }
   };

   class UTF8Character :
      public Parser,
      public std::string
   {

   private:
      std::vector<Char> _chars;
      Char _current;
      size_t _expectedCharCount;
   public:
      UTF8Character()
      {
      }

      UTF8Character(
         const std::string& bytes
      ) :
         std::string(bytes)
      {
      }

      virtual Optional read(bool bit)
      {

         Char& c = _current;

         if (c.read(bit) == true) {

            if (c._result == true) {
               _chars.push_back(c);
               if (_chars.size() == 1) {
                  _expectedCharCount =
                     getExpectedCharCount();
               }
               _current = Char();
               if (_chars.size() > 0 &&
                   _expectedCharCount ==
                      _chars.size())
               {
                  std::cout << "Hello Mother Fucker" << std::endl;
                  return read(*this);
               }
            }
            return true;
         }
         
         return false;
      }

      size_t getExpectedCharCount() {
         Char& first = _chars[0];
         size_t count;
         for (count = 0;
              first._bits[count]._bit;
              ++count)
         {
         }

         return count + 1;
      }

      std::wstring toWString() {

         std::wstring_convert<
            std::codecvt_utf8<wchar_t>
         > utf8Convert;

         std::wstring wstring =
            utf8Convert
               .from_bytes(*this);
         
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

   public:
      virtual Optional read(
         UTF8Character& character
      )
      {
         return false;
      }

   };


}


#endif
