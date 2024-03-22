#ifndef BEE_FISH_B_STRING__UTF_8_H
#define BEE_FISH_B_STRING__UTF_8_H

#include <vector>
#include <bitset>
#include <deque>
#include "../misc/optional.h"
#include <iomanip>

#include "character.h"

using namespace std;

namespace BeeFishBString {

   class UTF8Character
   {
   protected:
      unsigned int _expectedByteCount = 0;
      unsigned int _byteCount = 0;
      optional<bool> _result = nullopt;   
      
   public:
      typedef Character Value;
      Value _value;
      
      UTF8Character() :
         _expectedByteCount(0),
         _byteCount(0),
         _result(nullopt),
         _value("")
      {
      }

      UTF8Character(const Value& value) :
         _expectedByteCount(0),
         _byteCount(0),
         _result(nullopt),
         _value(value)

      {
      }
      
      UTF8Character(
         const UTF8Character& source
      ) :
         _expectedByteCount(source._expectedByteCount),
         _byteCount(source._byteCount),
         _result(source._result),
         _value(source._value)
      {
      }

      UTF8Character& operator = (const UTF8Character& rhs) {
         _expectedByteCount = rhs._expectedByteCount;
         _byteCount         = rhs._byteCount;
         _value             = rhs._value;
         _result            = rhs._result;

         return *this;
      }
      
      void reset()
      {
         _expectedByteCount = 0;
         _byteCount = 0;
         _value.clear();
         _result = nullopt;
      }
      
      Value& value()
      {
         return _value;
      }

      bool match(uint8_t byte)
      {
         bool matched = false;
         
         if (_byteCount == 0)
         {
            matched = matchFirstByte(byte);
         }
         else
         {
            matched = matchSubsequentBytes(byte);
         }
         
         if (matched)
         {
            // Check if we've read enough bytes.
            if ( ++_byteCount ==
                   _expectedByteCount )
            {
               // All bytes match
               _result = true;
            }
            
         }
         else {
            _result = false;
         }
            
         return matched;

         
      }
      
      bool matchFirstByte(const uint8_t byte)
      {
         bitset<8> bits(byte);
         
         // Match first byte
         for (auto it = FirstByte.cbegin();
                   it != FirstByte.cend();
                 ++it )
         {
            // Get the byte.
            UTF8Byte _byte = *it;
            
            // See if its a match.
            if (_byte.match(bits))
            {
               // Store how many subsequent bytes
               // (including this one).
               _expectedByteCount = _byte._byteCount;
               
               // Start the character value
               // using the bytes extract mask.
             
               _value = byte;
               
               
               return true;
            }
               
         }
         
         return false;
      }
      
      bool matchSubsequentBytes(const uint8_t byte)
      {
         bitset<8> bits(byte);
         
         // Match subsequent bytes.
         
         // Check the byte is a subsequent
         // byte.
         UTF8Byte _byte = UTF8Subsequent;
         if (_byte.match(bits))
         {
            // Left shift the 6 usable bits
            // onto the character value.
            _value.push_back(byte);
               
            // We matched.
            return true;
         }
         
         // We didn't
         return false;

      }

      BeeFishBString::Character character() {
         return _value;
      }      

      const optional<bool> result() const {
         return _result;
      }
                  
      
   protected:
   
      struct UTF8Byte
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
      
         bool match(const bitset<8>& bits)
         {

            bool matched =
               ((bits & _matchMask) == bits);
         
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
   

   };
 
}

#endif