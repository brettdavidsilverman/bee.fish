#ifndef BEE_FISH_B_STRING__CHARACTER_H
#define BEE_FISH_B_STRING__CHARACTER_H

#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <ctype.h>
#include <type_traits>

#include "../power-encoding/power-encoding.h"

using namespace std;
using namespace BeeFishPowerEncoding;

namespace BeeFishBString {

   typedef char32_t Character;
   inline void writeCharacter(
      ostream& out,
      const Character& value
   )
   {
      if (value <= 0x007F)
      {
         // 1 byte ascii character
         
         char c1 = (char)value;
         out << c1;
      }
      else if (value <= 0x07FF)
      {
         // 2 byte unicode
         
         //110xxxxx 10xxxxxx
         char c1 = ( 0b00011111         &
                     ( value >> 6 ) ) |
                     0b11000000;

         char c2 = ( 0b00111111  &
                     value ) |
                     0b10000000;
                           
         out << c1 << c2;
      }
      else if (value <= 0xFFFF)
      {
         // 3 byte unicode
         
         //1110xxxx 10xxxxxx 10xxxxxx
         char c1 = ( 0b00001111          &
                     ( value >> 12 ) ) |
                     0b11100000;
                        
         char c2 = ( 0b00111111          &
                     ( value >>  6 ) ) |
                     0b10000000;
                        
         char c3 = ( 0b00111111  &
                     value ) |
                     0b10000000;
                        
         out << c1 << c2 << c3;

      }
      else if (value <= 0x10FFFF)
      {
         // 4 byte unicode
         
         //11110xxx 10xxxxxx
         //10xxxxxx 10xxxxxx
         char c1 = ( 0b00000111         &
                     ( value >> 18) ) |
                     0b11110000;
                           
         char c2 = ( 0b00111111         &
                     ( value >> 12) ) |
                     0b10000000;
                           
         char c3 = ( 0b00111111         &
                     ( value >>  6) ) |
                     0b10000000;
                           
         char c4 = ( 0b00111111 &
                     value ) |
                     0b10000000;

         out << c1 << c2 << c3 << c4;
      }
      else
      {
         // Invalid character
         out << "\\u" 
                  << std::hex
                  << std::setw(4)
                  << std::setfill('0')
                  <<
                  ((value & 0xFFFF0000)
                     >> 16); //15
         out << "\\u" 
                  << std::hex
                  << std::setw(4)
                  << std::setfill('0')
                  <<
                  (value & 0x0000FFFF);
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
      // std::ios_base::fmtflags f( out.flags() );
      switch (value)
      {
      case '\"':
         out << "\\\"";
         break;
      case '\\':
         out << "\\\\";
         break;
      case '\b':
         out << "\\b";
         break;
      case '\f':
         out << "\\f";
         break;
      case '\r':
         out << "\\r";
         break;
      case '\n':
         out << "\\n";
         break;
      case '\t':
         out << "\\t";
         break;
      default:
         if (value <= 0x001F)
            out << "\\u" 
                  << std::hex
                  << std::setw(4)
                  << std::setfill('0')
                  << (uint32_t)value;
         else if (value > 0x10FFFF)
         {
            out << "\\u" 
                  << std::hex
                  << std::setw(4)
                  << std::setfill('0')
                  << 
                  ((value & 0xFFFF0000) >>
                     16);
            out << "\\u" 
                  << std::hex
                  << std::setw(4)
                  << std::setfill('0')
                  <<
                  (value & 0x0000FFFF);
         }
         else {
            writeCharacter(out, value);
         }
      }
      
      // out.flags( f );
   }
      
   // https://unicodebook.readthedocs.io/unicode_encodings.html#surrogates
   inline bool isSurrogatePair(
      const Character& first,
      const Character& second
   )
   {
      return ( ( 0xD800 <= first && 
                  first <= 0xDBFF ) &&
               ( 0xDC00 <= second &&
                  second <= 0xDFFF) );
   }
   
   inline Character& joinSurrogatePair(
      Character& first,
      const Character& second
   )
   {
      Character value = 0x10000;
      
      value += (first & 0x03FF) << 10;
      value += (second & 0x03FF);
      
      first = value;
      
      return first;
   }
   

}

#endif


