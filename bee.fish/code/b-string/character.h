#ifndef BEE_FISH_B_STRING__CHARACTER_H
#define BEE_FISH_B_STRING__CHARACTER_H

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
    class Character : public std::string
   {
   public:
      Character() {
      }
      
      Character(char c) {
         push_back(c);
      }
      
      Character(const char* character) :
         std::string(character)
      {
      }
      
      Character(const std::string& character) :
         std::string(character)
      {
      }
      
      friend PowerEncoding& operator <<      (
         PowerEncoding& out,
         const Character& value
      )
      {
          out << (std::string&)value;
          return out;
      }
      
      friend PowerEncoding& operator >>      (
         PowerEncoding& in,
         Character& value
      )
      {
          in >> (std::string&)value;
          return in;
      }
      
      
   };
   

   inline void writeCharacter(
      ostream& out,
      const Character& value
   )
   {
      for (const char c : value) {
         out << c;
      }
   }
      
   inline ostream& operator << (ostream& out, const Character& character) {
      writeCharacter(out, character);
      return out;
   }

   inline void writeEscapedCharacter(
      ostream& out,
      const Character& value
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


