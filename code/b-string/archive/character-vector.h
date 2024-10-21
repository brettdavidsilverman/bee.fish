#ifndef BEE_FISH_B_STRING__CHARACTER_H
#define BEE_FISH_B_STRING__CHARACTER_H

#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <ctype.h>
#include <vector>

#include "../power-encoding/power-encoding.h"

using namespace std;
using namespace BeeFishPowerEncoding;

namespace BeeFishBString {

   typedef vector<bool> CharacterBase;
   
   class Character :
      public CharacterBase
   {
   protected:
      
      class Encoder :
         public PowerEncoding
      {
      private:
         Character& _character;
         
      public:
         Encoder(
            Character& character
         ) :
            _character(character)
         {
         }
            
         virtual void writeBit(bool bit)
         {
            PowerEncoding::writeBit(bit);
            _character.push_back(bit);
         };
      
      };
      
      class Decoder :
         public PowerEncoding
      {
      private:
         const Character& _character;
         Character::const_iterator _it;
      public:
         Decoder(
            const Character& character
         ) :
            _character(character),
            _it(_character.cbegin())
         {
         }
                  
         virtual bool readBit()
         {
            if (_it == _character.cend())
               throw runtime_error("Character past end of file");
               
            bool bit = PowerEncoding::readBit();
            ++_it;
            return bit;
         };
      
         virtual bool peekBit()
         {
            if (_it == _character.cend())
               return 0;

            return *_it;
         }
      };
         
   public:
      typedef uint32_t Value;
      
      Character() : Character(0)
      {
      }
      
      Character(const Character& source) :
         CharacterBase(source)
      {

      }
      
      Character(const CharacterBase& source) :
         CharacterBase(source)
      {
      }

      Character(const initializer_list<bool>& values) : CharacterBase(values) {

      }
      
      Character(Value value)
      {
         Encoder encoder(*this);
         encoder.writeBit(1);
         encoder << value;
      }

      virtual ~Character()
      {
      }
      
      Character& operator = (const Character& source)
      {
          vector<bool>::operator = (source);
          
          return *this;
      }
      
      operator Value () const
      {
         return value();
      }
       
      Value value() const {
         Decoder decoder(*this);
         Value value;
         
         assert(decoder.readBit() == 1);

         decoder >> value;
         return value;
      }

      Character& operator++() {
         Value value = Character::value();
         ++value;
         (*this) = value;

         return (*this);
      }
      
      // Character bits are already power
      // encoded, so simply write the bits
      // to the encoding
      friend PowerEncoding& 
      operator <<
      (
         PowerEncoding& encoding,
         const Character& character
      )
      {
         for (auto bit : character)
         {
            encoding.writeBit(bit);
         }
      
         return encoding;
      }
      
      // Extract bits from encoding,
      // keeping track of +/- count.
      // Finish when count reaches zero.
      friend PowerEncoding&
      operator >>
      (
         PowerEncoding& encoding,
         Character& character
      )
      {
         character.clear();
         
         bool bit;
         
         do
         {
            bit = encoding.readBit();
            character.push_back(bit);
         }
         while (encoding.count() > 0);


         return encoding;
      }
      
      Character toLower() const
      {
         Value character = *this;
         Value lower = tolower(character);
         return Character(lower);
      }
      
      // Defined in misc.h
      BString bstr() const;

      static void write(
         ostream& out,
         const Value& value
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
      


      void writeEscaped(
         ostream& out
      ) const
      {
        // std::ios_base::fmtflags f( out.flags() );
         Value value = *this;
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
                   << value;
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
               Character::write(out, value);
            }
         }
         
      }

      virtual void write(ostream& out) const
      {
         Character::write(
            out,
            (Value)(*this)
         );
      }
      
      friend ostream& operator <<
      (ostream& out, const Character& character)
      {
         character.write(out);
         
         return out;
      }
      
      
      // https://unicodebook.readthedocs.io/unicode_encodings.html#surrogates
      bool isSurrogatePair(
         const Value second
      )
      {
         const Value first = *this;
         
         return ( ( 0xD800 <= first && 
                    first <= 0xDBFF ) &&
                  ( 0xDC00 <= second &&
                    second <= 0xDFFF) );
      }
      
      Character& joinSurrogatePair(
         const Value second
      )
      {
         Value first = *this;
         
         Value value = 0x10000;
         
         value += (first & 0x03FF) << 10;
         value += (second & 0x03FF);
         
         Character character = value;
         
         clear();
         
         for ( auto bit : character )
         {
            push_back(bit);
         }

         return *this;
      }
      
     /*
      virtual void writeBit(bool bit)
      {
         push_back(bit);
      };
      
      virtual bool readBit() 
      {
         throw logic_error("Not implemented");
      }
      
      virtual bool peekBit()
      {
         throw logic_error("Not implemented");
      }
      */
      
     /*
      static Character fromHex(const string& hex)
      {
         Character result;
         
         std::stringstream stream;
         stream << std::hex << hex;
         stream >> result._character;
         
         return result;
      }
      string toHex() const
      {
         stringstream stream;
         stream
            << std::hex
            << std::setw(8)
            << std::setfill('0')
            << std::uppercase
            << _character;
         return stream.str();
      }
      */

   };
   /*
   
   inline double log2(const Character& character)
   {
      return log2(character._character);
   }
   */

/*
  inline bool operator == (const Character& lhs, const Character& rhs)  {
     return (lhs.value() == rhs.value());
  }
  inline bool operator != (const Character& lhs, const char& rhs)  {
     return (lhs.value() != (unsigned int)rhs);
  }
*/
}

#endif


