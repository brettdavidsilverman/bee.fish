#ifndef BEE_FISH_B_STRING__CHAR_H
#define BEE_FISH_B_STRING__CHAR_H

#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <ctype.h>
#include <type_traits>

#include "../power-encoding/power-encoding.h"
#include "../power-encoding/encoding.h"#include "../Miscellaneous/Miscellaneous.hpp"

using namespace std;
using namespace BeeFishPowerEncoding;

namespace BeeFishBString {
    class Char : public std::string
   {
   public:
      Char() {
      }
      
      Char(char c) {
         push_back(c);
      }
      
      Char(const char* character) :
         std::string(character)
      {
      }
      
      Char(const std::string& character) :
         std::string(character)
      {
      }
      
      friend PowerEncoding& operator <<      (
         PowerEncoding& out,
         const Char& value
      )
      {
          out << (std::string&)value;
          return out;
      }
      
      friend PowerEncoding& operator >>      (
         PowerEncoding& in,
         Char& value
      )
      {
          in >> (std::string&)value;
          return in;
      }
      
      
   };
   

   inline void writeChar(
      ostream& out,
      const Char& value
   )
   {
      for (const char c : value) {
         out << c;
      }
   }
      
   inline ostream& operator << (ostream& out, const Char& character) {
      writeChar(out, character);
      return out;
   }

   inline void writeEscapedChar(
      ostream& out,
      const Char& value
   )
   {
      for (auto c : value)
      {
         out << BeeFishMisc::escape(c);
      }
      
      return;
      
      
   }
      
}

#endif


