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

#include "../parser/utf-8.h"

#ifdef SERVER
using namespace std::filesystem;
#endif

namespace BeeFishBString
{


   //typedef vector<Character> BStringBase;
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
   protected:
      std::string _buffer;
      
   public:
      typedef Character ValueType;
   
      // empty string
      BString()
      {
      }

      // standard copy constructor
      BString(const BString &source) : BStringBase(source)
      {
      }

      // from vector
      BString(const BStringBase &source) : BStringBase(source)
      {
      }

   
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
         BeeFishParser::UTF8Character utf8;
         BString result;
         for (auto character : str)
         {
            if (utf8.match(character))
            {
               if (utf8.result() == true)
               {
                  result.push_back(utf8.character());
                  utf8.reset();
               }
            }
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
            this->push_back(c);
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
         std::string buffer;
         stringstream stream;
         stream << *this;
         buffer = stream.str();
         return buffer;
      }

      operator const char*() {
         return c_str();
      }

      const char* c_str() {
         stringstream stream;
         stream << *this;
         _buffer = stream.str();
         return _buffer.c_str();
      }

      const Character* data() const {
         return BStringBase::c_str();
      }

      virtual ~BString()
      {
      }

      size_t length() const {
         return size();
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
                tolower(c));

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

         stream.writeBit(true);

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

         while (stream.peakBit() == 1)
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

      friend istream &getline(istream &in, BString &line)
      {
         string str;
         getline(in, str);
         line = BString::fromUTF8String(str);
         return in;
      }
   };

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
