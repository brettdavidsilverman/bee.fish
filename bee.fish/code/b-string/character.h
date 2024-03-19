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

   inline PowerEncoding& operator << (PowerEncoding& encoding, const Character& value) {
      encoding.writeBit(1);

      encoding << (unsigned long)value;

      return encoding;
   }

   inline PowerEncoding& operator >> (PowerEncoding& encoding, Character& character) {
      assert(encoding.readBit() == 1);
      unsigned long value;
      encoding >> value;

      character = value;

      return encoding;
   }

   inline std::string getChars(const Character& value) {

      std::string buffer;

      if (value <= 0x007F)
      {
         // 1 byte ascii character
         
         char c1 = (char)value;
         buffer = c1;
         return buffer;
      }
      else if (value <= 0x07FF)
      {
         
         // 2 byte unicode
         
         //110xxxxx 10xxxxxx
         char c1 = ( 0b00011111  &
                     ( value >> 6 ) ) |
                     0b11000000;

         char c2 = ( 0b00111111  &
                     value ) |
                     0b10000000;
                           
         buffer = c1;
         buffer += c2;

//         cerr << "{" << hex << (int)c1 << ", " << hex << (int)c2 << "}";

         return buffer;
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
                        
         buffer = c1;
         buffer += c2;
         buffer += c3;

         return buffer;
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

         buffer = c1;
         buffer += c2;
         buffer += c3;
         buffer += c4;

         return buffer;
      }
      else
      {
         // Invalid character
         std::stringstream stream;

         stream << "\\u" 
                  << std::hex
                  << std::setw(4)
                  << std::setfill('0')
                  <<
                  ((value & 0xFFFF0000)
                     >> 16); //15
         stream << "\\u" 
                  << std::hex
                  << std::setw(4)
                  << std::setfill('0')
                  <<
                  (value & 0x0000FFFF);

         return stream.str();
      }


   }

   inline void writeCharacter(
      ostream& out,
      const Character& value
   )
   {
      const std::string chars = getChars(value);
      for (const char c : chars) {
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
         
         //cerr << value ;
         //cerr << "{" << value << "," << std::hex << (uint32_t)value << "}";

         if (value <= 0x001F) {
            // Control chars
            out << "\\u" 
                  << std::hex
                  << std::setw(4)
                  << std::setfill('0')
                  << (uint32_t)value;

         }
         else if (value > 0x10FFFF)
         {
            // Uhicode chars
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


