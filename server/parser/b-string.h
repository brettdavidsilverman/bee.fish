#ifndef BEE_FISH_PARSER__B_STRING_H
#define BEE_FISH_PARSER__B_STRING_H

#include <iostream>
#include <string>
#include "utf-8.h"

namespace bee::fish::parser {

   
   class BString :
      public vector<Char>
   
   {
   public:
      // empty string
      BString()
      {
      }
      
      // utf-8 string 
      BString(const std::string& string)
      {
         UTF8Character utf8;
         
         for (const char& c : string)
         {
            utf8.match(c);
            if (utf8._result)
            {
               push_back(utf8._character);
               utf8.reset();
            }
         }

      }
      
      // wide string 
      BString(const std::wstring& wstring)
      {
         for (const wchar_t& wc : wstring)
         {
            push_back((Char)wc);
         }
      }
      
      // utf-8 character
      BString(const Char& character)
      {
         push_back(character);
      }
   
      friend ostream& operator <<
      (ostream& out, const BString& bstr)
      {
         for (auto c : bstr)
         {
            UTF8Character::writeUTF8(out, c);
         }
         
         return out;
      }
      
      bool operator == (const string& rhs)
      {
         BString test(rhs);
         return (*this == test);
      }
      
      bool operator != (const string& rhs)
      {
         BString test(rhs);
         return (*this != test);
      }
      
      BString& operator += (const string& rhs)
      {
         BString string(rhs);
         insert(end(), string.begin(), string.end());
         return *this;
      }
      
   };
   
   
}

#endif


