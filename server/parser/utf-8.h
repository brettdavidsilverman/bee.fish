#ifndef BEE_FISH_PARSER__UTF_8_H
#define BEE_FISH_PARSER__UTF_8_H

#include <bitset>
#include <wchar.h>

#include "match.h"
#include "character.h"

namespace bee::fish::parser {
   
   struct UTF8Byte : public Match
   {
      bitset<8>  _matchMask;
      bitset<8>  _extractMask;
      size_t     _byteCount;
      
      UTF8Byte(
         bitset<8> matchMask,
         bitset<8> extractMask,
         size_t    byteCount
      ) :
         _matchMask(matchMask),
         _extractMask(extractMask),
         _byteCount(byteCount)
      {
      }
      
      UTF8Byte(
         const UTF8Byte& source
      ) :
         _matchMask(source._matchMask),
         _extractMask(source._extractMask),
         _byteCount(source._byteCount)
      {
      }
      
      virtual Match* copy() const
      {
         return new UTF8Byte(*this);
      }
      
      virtual void write(ostream& out) const
      {
         out << "UTF8Byte";
         writeResult(out);
         out << "({" << _matchMask << "}, "
             << "{" << _extractMask << "}, "
             << _byteCount << ")";
      }
      
      virtual bool match(const Char& character)
      {
         bitset<8> bits(character);
         return match(bits);
      }
      
      virtual bool match(const bitset<8>& bits)
      {

         bool matched =
            ((bits & _matchMask) == bits);
         
         if (matched)
            success();
         else
            fail();
            
         return matched;
      }
      
      
   };
   
   const vector<UTF8Byte> FirstByte{
      UTF8Byte(
         0b01111111,
         0b01111111,
         1
      ),
      UTF8Byte(
         0b11011111,
         0b00011111,
         2
      ),
      UTF8Byte(
         0b11101111,
         0b00001111,
         3
      ),
      UTF8Byte(
         0b11110111,
         0b00000111,
         4
      )
   };
   
   const UTF8Byte UTF8Subsequent = 
      UTF8Byte(
         0b10111111,
         0b00111111,
         1
      );
   
   
   class UTF8Character :
      public Character
   {
   protected:
      unsigned int _firstByteCount;
      unsigned int _byteCount;
      Char         _temporary;
   public:
   
      UTF8Character() :
         Character(),
         _firstByteCount(0),
         _byteCount(0),
         _temporary(0)
      {
         
      }
       
      UTF8Character(Char& character) :
         Character(character),
         _firstByteCount(0),
         _byteCount(0),
         _temporary(0)
      {
      }
      
      UTF8Character(
         const UTF8Character& source
      ) :
         Character(source),
         _firstByteCount(source._firstByteCount),
         _byteCount(source._byteCount),
         _temporary(source._temporary)
      {
      }
      
      virtual bool match(const Char& character)
      {
         bitset<8> bits(character);
         bool matched = false;
         
         if (_byteCount == 0)
         {
            matched = matchFirstByte(bits);
         }
         else
         {
            matched = matchSubsequent(bits);
         }
         
         if (matched)
         {
            // Check if we've read enough bytes.
            if ( ++_byteCount ==
                   _firstByteCount )
            {
               // All bytes match
               // Get character to perform
               // further checks before calling
               // success.
               Character::match(
                  _temporary
               );
               
            }
            
         }
         else
            fail();
            
         return matched;

         
      }
      
      bool matchFirstByte(const bitset<8>& bits)
      {
         // Match first byte
         for (auto it = FirstByte.cbegin();
                   it != FirstByte.cend();
                 ++it )
         {
            // Get the byte.
            UTF8Byte byte = *it;
            
            // See if its a match.
            if (byte.match(bits))
            {
               // Store how many subsequent bytes
               // (including this one).
               _firstByteCount = byte._byteCount;
               
               // Start the character value
               // using the bytes extract mask.
             
               _temporary = (
                  bits & byte._extractMask
               ).to_ulong();
               
               
               return true;
            }
               
         }
         
         return false;
      }
      
      bool matchSubsequent(const bitset<8> bits)
      {
         // Match subsequent bytes.
         
         // Check the byte is a subsequent
         // byte.
         UTF8Byte byte = UTF8Subsequent;
         if (byte.match(bits))
         {
            // Left shift the 6 usable bits
            // onto the character value.
            _temporary =
               (_temporary << 6) |
               (
                  bits &
                     UTF8Subsequent._extractMask
               ).to_ulong();
               
            // We matched.
            return true;
         }
         
         // We didn't
         return false;

      }
      
      virtual Match* copy() const
      {
         return new UTF8Character(*this);
      }
      
      virtual void write(ostream& out) const
      {
         out << "UTF8Character";
         writeResult(out);
         out << "(";
         if (!_matchAny)
         {
            out << character();
         }
         out << ")";
      }
      
      static void writeCharacter(ostream& out, const Char& character)
      {
         switch (character)
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
         case Match::EndOfFile:
            out << "-1";
            break;
         default:
            writeUTF8Character(out, character);
         }
      }
      
      static void writeUTF8Character(
         ostream& out,
         const Char& character
      )
      {
         if (character <= 0x007F)
         {
            char c1 = (char)character;
            out << c1;
         }
         else if (character <= 0x07FF)
         {
            //110xxxxx 10xxxxxx
            char c1 = ( 0b00011111         &
                      ( character >> 6 ) ) |
                        0b11000000;
                            
            char c2 = ( 0b00111111  &
                        character ) |
                        0b10000000;
                            
            out << c1 << c2;
         }
         else if (character <= 0xFFFF)
         {
            //1110xxxx 10xxxxxx 10xxxxxx
            char c1 = ( 0b00001111          &
                      ( character >> 12 ) ) |
                        0b11100000;
                           
            char c2 = ( 0b00111111          &
                      ( character >>  6 ) ) |
                        0b10000000;
                           
            char c3 = ( 0b00111111  &
                        character ) |
                        0b10000000;
                           
            out << c1 << c2 << c3;

         }
         else if (character <= 0x10FFFF)
         {
            //11110xxx 10xxxxxx
            //10xxxxxx 10xxxxxx
            char c1 = ( 0b00000111         &
                      ( character >> 18) ) |
                        0b11110000;
                            
            char c2 = ( 0b00111111         &
                      ( character >> 12) ) |
                        0b10000000;
                            
            char c3 = ( 0b00111111         &
                      ( character >>  6) ) |
                        0b10000000;
                            
            char c4 = ( 0b00111111 &
                        character ) |
                        0b10000000;
                            
            out << c1 << c2 << c3 << c4;
         }
         else if (character == Match::EndOfFile)
         {
            out << "{-1}";
         }
         else
         {
            out << "{"
                << (int32_t)character
                << "}";
         }
   
      }
   };
 
}

#endif