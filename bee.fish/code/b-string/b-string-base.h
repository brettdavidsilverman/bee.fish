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
#include <codecvt>

#ifdef SERVER
#include <filesystem>
#endif

#include "char.h"

#include "utf-8.h"

#ifdef SERVER
using namespace std::filesystem;
#endif

namespace BeeFishBString
{


   //typedef vector<Char> BStringBase;
   typedef uint8_t Byte;
   
   typedef std::string BStringBase;

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
      typedef Char ValueType;
   
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

      // c string
      BString(const char *cstring) : BString(std::string(cstring))
      {
      }

      // c string with length
      BString(const char *cstring, size_t len) : BString(std::string(cstring, len))
      {
      }


      std::string str() const {
         stringstream stream;
         stream << *this;
         return stream.str();
      }

      void push_back(const Char &character)
      {
         *this += character;
      }

      void push_back(char c)
      {
         BStringBase::push_back(c);
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
         std::string::operator += (rhs);
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
      
      BString &operator+=(const char &rhs)
      {
         std::string::operator += (rhs);
         return *this;
      }
      

      virtual BString& operator = (const BString& rhs) {
         BStringBase::operator = (rhs);
         //_utf8 = rhs._utf8;
         return *this;
      }

      bool startsWith(const BString& prefix) const {
         return (rfind(prefix, 0) == 0);
      }


      BString toLower() const
      {
         BString copy;
         for (const char &c : *this)
            copy.push_back(
                tolower(c)
            );

         return copy;
      }

      vector<BString> split(
         const char character
      ) const
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

      virtual void writeEscaped(
          ostream &out
      ) const
      {
         for (auto character : *this)
         {
            out << BeeFishMisc::escape(character);
         }
      }


      friend PowerEncoding &operator>>(
          PowerEncoding &stream,
          BString &bString)
      {
         size_t startCount = stream._count;
         
         if (stream.readBit() != 1)
            throw runtime_error("Invalid stream");
            
         bString.clear();
         unsigned int character;

         while (stream.peekBit() == 1)
         {
            stream >> character;
            bString.push_back((char)character);
         }

         if (stream.readBit() != 0)
            throw runtime_error("Invalid stream");
            
         size_t endCount = stream._count - startCount;
         
         assert(endCount == 0);
         
         return stream;
      }
      
      friend PowerEncoding &operator<<(
          PowerEncoding &stream,
          const BString &bString)
      {
         size_t startCount = stream._count;
         
         stream.writeBit(1);
         uint8_t character;

         for (char c : bString)
         {
            character = (uint8_t)c;
            stream << character;
         }
         
         stream.writeBit(0);
         
         size_t endCount = stream._count - startCount;
         
         assert(endCount == 0);
         
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
                 [](char ch)
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
                 [](char ch)
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
      
      bool contains(const Char& character) const
      {
         if (find(character) != std::string::npos)
            return true;
            
         return false;
      }
      
      bool contains(char character) const
      {
         if (find(character) != std::string::npos)
            return true;
            
         return false;
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

         ostringstream escaped;
         escaped.fill('0');
         escaped << hex;

         for (std::string::const_iterator i = cbegin(), n = cend(); i != n; ++i) {
            char c = (*i);

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
         char character;
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
               std::string part = encoded.substr(i + 1, 2);
               sscanf(part.c_str(), "%x", &ii);
               character = static_cast<char>(ii);
               decoded.push_back(character, utf8);
               i = i + 2;
            }
         }

         return decoded;

      }

      friend istream &getline(istream &in, BString &line)
      {
         string str;
         getline(in, str);
         line = str;
         return in;
      }
      
      // convert UTF-8 string to wstring
      std::wstring utf8_to_wstring ()
      {
         std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
         return myconv.from_bytes(*this);
      }
      
      // convert wstring to UTF-8 string
      static BString wstring_to_utf8 (const std::wstring& str)
      {
         std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
         return myconv.to_bytes(str);
      }
      
      BString toLower() {
         // the locale will be the UTF-8 enabled English
         std::setlocale(LC_CTYPE, "en_US.UTF-8");
         
         std::wstring str = utf8_to_wstring();

         for (std::wstring::iterator it = str.begin(); it != str.end(); ++it)
         {
            *it = towlower(*it);
         }
         
         return wstring_to_utf8(str);
      }
      
      BString toUpper() {
         // the locale will be the UTF-8 enabled English
         std::setlocale(LC_CTYPE, "en_US.UTF-8");
         
         std::wstring str = utf8_to_wstring();

         for (std::wstring::iterator it = str.begin(); it != str.end(); ++it)
         {
            *it = towupper(*it);
         }
         
         return wstring_to_utf8(str);
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
