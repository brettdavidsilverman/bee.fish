#ifndef BEE_FISH_B_STRING__B_STRING_H
#define BEE_FISH_B_STRING__B_STRING_H

#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <ctype.h>
#include <cstring>
#include <algorithm>
#include <cctype>
#include <locale>
#include <codecvt>
#include <locale>

#ifdef SERVER
#include <filesystem>
#endif

#include "character.h"

#include "utf-8.h"

#ifdef SERVER
using namespace std::filesystem;
#endif

namespace BeeFishBString
{


   //typedef vector<Character> BStringBase;
   typedef uint8_t Byte;
   
   typedef std::basic_string<Character> BStringBase;

   class BString;
   class Data;
   
   // A string of variable length characters.
   // Can be created from wide string format,
   // and utf-8 format.
   // Once created, holds each character as a
   // power encoded vector of bits
   class BString : public BStringBase
   {
      
   public:
      typedef Character ValueType;
   
      // empty string
      BString()
      {
      }

      // standard copy constructor
      BString(const BString &source) : 
         BStringBase(source)
      {
      }

      // from vector
      BString(const BStringBase &source) : BStringBase(source)
      {
      }

      virtual ~BString() {

      }

      // defined in misc.h
      Data toData() const;

      // defined in misc.h
      static BString fromData(const Data &source);

      // from  Data, defined in misc.h
      //BString(const Data &source);

     
#ifdef SERVER
      // from path
      BString(const std::filesystem::path &path) : BString(string(path))
      {
      }
#endif

      // from copy iterators
      BString(
          BStringBase::const_iterator first,
          BStringBase::const_iterator last) : BStringBase(first, last)
      {
      }

      static BString fromUTF8String(const std::string &str)
      {
         BString result;
         UTF8Character utf8;

         for (const char character : str)
         {
            result.push_back((uint8_t)character, utf8);
         }

         return result;
      }

      // from utf-8 string
      BString(const std::string &str) : BString(fromUTF8String(str))
      {
      }

      // c string
      BString(const char *cstring) : BString(std::string(cstring))
      {
      }

      // c string with length
      BString(const char *cstring, size_t len) : BString(std::string(cstring, len))
      {
      }

      BString(wstring wstr)
      {

         for (auto character : wstr)
         {
            Character c = character;
            push_back(c);
         }
      }

      // Wide c string
      BString(const wchar_t *wstr) : BString(wstring(wstr))
      {
      }

      // from character
      BString(const Character &character) : BStringBase{character}
      {
      }

   /*
      // Stream indexable bits from data
      static BString fromData(const Data &source)
      {

         BitStream stream = BitStream::fromData(source);

         BString bString;

         stream >> bString;

         return bString;
      }
*/
      std::string str() const {
         stringstream stream;
         stream << *this;
         return stream.str();
      }

      void push_back(const Character &character)
      {
         if (size())
         {
            Character &last = (*this)[size() - 1];
            if (isSurrogatePair(last, character))
            {
               joinSurrogatePair(last, character);
               return;
            }
         }
         BStringBase::push_back(character);
      }

      void push_back(uint32_t character, UTF8Character& utf8) {

         uint32_t largeByte = character;

         uint8_t firstByte  =  largeByte >> 24;
         uint8_t secondByte = (largeByte & 0x00FF0000) >> 16;
         uint8_t thirdByte  = (largeByte & 0x0000FF00) >> 8;
         uint8_t fourthByte = (largeByte & 0x000000FF);


         if (firstByte > 0) {
            BString::push_back(firstByte,  utf8);
            BString::push_back(secondByte, utf8);
            BString::push_back(thirdByte,  utf8);
            BString::push_back(fourthByte, utf8);
         }
         else if (secondByte > 0) {
            BString::push_back(secondByte, utf8);
            BString::push_back(thirdByte,  utf8);
            BString::push_back(fourthByte, utf8);
         }
         else if (thirdByte > 0) {
            BString::push_back(thirdByte,  utf8);
            BString::push_back(fourthByte, utf8);
         }
         else
            BString::push_back(fourthByte, utf8);
      }

      void push_back(uint8_t byte, UTF8Character& utf8)
      {
         if (utf8.match(byte))
         {
            if (utf8.result() == true)
            {
               push_back(utf8.character());
               utf8.reset();
               return;
            }
         }

         if (utf8.result() == false) {
            throw std::runtime_error("Invalid utf-8 character");
         }
      }

      BString &operator+=(const BString &rhs)
      {
         for (auto character : rhs)
         {
            push_back(character);
         }
         return *this;
      }

      BString operator+(const BString &rhs) const
      {
         BString str(*this);
         str.reserve(this->size() + rhs.size());
         str += rhs;
         return str;
         /*
         for (auto character : rhs)
            str.push_back(character);
         
         return str;
         */
      }

      virtual BString& operator = (const BString& rhs) {
         BStringBase::operator = (rhs);
         //_utf8 = rhs._utf8;
         return *this;
      }

      virtual bool operator ==(const Character& character) const
      {
         return ((size() == 1) && (this->at(0) == character));
      }

      operator const std::string()
      {
         return str();
      }

      bool startsWith(const BString& prefix) const {
         return (rfind(prefix, 0) == 0);
      }


      BString toLower() const
      {
         BString copy;
         for (const Character &c : *this)
            copy.push_back(
                (Character)tolower(c)
            );

         return copy;
      }

      vector<BString> split(
          const Character &character) const
      {
         BString segment;
         vector<BString> segments;

         for (auto c : *this)
         {

            if (c == character)
            {
               segments.push_back(segment);
               segment.clear();
            }
            else
               segment.push_back(c);
         }

         segments.push_back(segment);

         return segments;
      }

      friend ostream &operator<<(ostream &out, const BString &str)
      {
         str.write(out);
         return out;
      }

      virtual void write(ostream &out) const
      {
         for (const Character &character : *this)
         {
            writeCharacter(out, character);
         }
      }

      virtual void writeEscaped(
          ostream &out) const
      {
         for (const Character &character : *this)
            writeEscapedCharacter(out, character);
      }

      friend PowerEncoding &operator<<(
          PowerEncoding &stream,
          const BString &bString)
      {

         stream.writeBit(1);

         for (auto character : bString)
         {
            stream << character;
         }

         stream.writeBit(0);

         return stream;
      }

      friend PowerEncoding &operator>>(
          PowerEncoding &stream,
          BString &bString)
      {
         assert(stream.readBit() == 1);

         bString.clear();
         Character character;

         while (stream.peekBit() == 1)
         {
            stream >> character;
            bString.push_back(character);
         }

         assert(stream.readBit() == 0);

         return stream;
      }

      // trim from start
      BString ltrim()
      {
         BString s = *this;
         s.erase(
             s.begin(),
             std::find_if(
                 s.begin(),
                 s.end(),
                 [](Character ch)
                 {
                    return !std::isspace(ch);
                 }));
         return s;
      }

      // trim from end (in place)
      BString rtrim()
      {
         BString s = *this;

         s.erase(
             std::find_if(
                 s.rbegin(),
                 s.rend(),
                 [](Character ch)
                 {
                    return !std::isspace(ch);
                 })
                 .base(),
             s.end());

         return s;
      }

      bool endsWith(const BString& ending) const {
          if (ending.size() > size())
            return false;
         return std::equal(ending.rbegin(), ending.rend(), rbegin());
      }

      BString trim()
      {
         return ltrim().rtrim();
      }

      BString substr(size_t pos, size_t len = 0) const
      {
         BStringBase::const_iterator
             start = cbegin() + pos;

         BStringBase::const_iterator
             end;

         if (len)
            end = start + len;
         else
            end = cend();
         return BString(start, end);
      }

      BString encodeURI() const {
         
         using namespace std;

         std::string string = this->str();

         ostringstream escaped;
         escaped.fill('0');
         escaped << hex;

         for (std::string::const_iterator i = string.cbegin(), n = string.cend(); i != n; ++i) {
            std::string::value_type c = (*i);

            // Keep alphanumeric and other accepted characters intact
            if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
                  escaped << c;
                  continue;
            }

            // Any other characters are percent-encoded
            escaped << uppercase;
            escaped << '%' << setw(2) << int((unsigned char) c);
            escaped << nouppercase;
         }

         return escaped.str();
      }         
      BString decodeURI() const{
         BString decoded;
         Character character;
         const BString& encoded = *this;
         int i, ii, len = length();
         UTF8Character utf8;

         for (i=0; i < len; i++) {
            if (encoded[i] != '%') {
               //if((*this)[i] == '+')
               //   decoded.push_back(' ');
               //else
               decoded.push_back(encoded[i], utf8);
            }
            else {
               std::string part = encoded.substr(i + 1, 2).str();
               sscanf(part.c_str(), "%x", &ii);
               character = static_cast<Character>(ii);
               decoded.push_back((uint32_t)character, utf8);
               i = i + 2;
            }
         }

         return decoded;

      }

      friend istream &getline(istream &in, BString &line)
      {
         string str;
         getline(in, str);
         line = BString::fromUTF8String(str);
         return in;
      }
   };

   inline bool operator==(const char* _1, const BStringBase& _2)
   {
      return BString(_1) == BString(_2);
   }

   inline bool operator==(const BStringBase& _1, const char *_2)
   {
      return BString(_1) == BString(_2);
   }

   inline bool operator!=(const BStringBase& _1, const char *_2)
   {
      return BString(_1) != BString(_2);
   }


   inline BString operator + (const char* bstr1, const BString& bstr2) {
      BString _bstr = bstr1;
      return _bstr + bstr2;
   }
}

#endif
