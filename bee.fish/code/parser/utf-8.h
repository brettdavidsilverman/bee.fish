#ifndef BEE_FISH_PARSER__UTF_8_H
#define BEE_FISH_PARSER__UTF_8_H

#include <vector>
#include <bitset>
#include <deque>
#include "../misc/optional.h"
#include <iomanip>

using namespace std;

namespace BeeFishParser {

   class UTF8Character
   {
   protected:
      unsigned int _expectedByteCount = 0;
      unsigned int _byteCount = 0;
      BeeFishMisc::optional<bool> _result = BeeFishMisc::nullopt;   
      
   public:
      typedef uint32_t Value;
      Value _value = 0;
      
      UTF8Character()
      {
         _value = 0;
         _result = BeeFishMisc::nullopt;
      }

      UTF8Character(const Value& value) {
         _value = value;
         _result = BeeFishMisc::nullopt;
      }
      
      UTF8Character(
         const UTF8Character& source
      ) :
         _expectedByteCount(source._expectedByteCount),
         _byteCount(source._byteCount),
         _value(source._value)
      {
         _result = BeeFishMisc::nullopt;
      }
      
      void reset()
      {
         _expectedByteCount = 0;
         _byteCount = 0;
         _value = 0;
         _result = BeeFishMisc::nullopt;
      }
      
      Value& value()
      {
         return _value;
      }

      bool match(uint8_t byte)
      {
         bitset<8> bits(byte);
         bool matched = false;
         
         if (_byteCount == 0)
         {
            matched = matchFirstByte(bits);
         }
         else
         {
            matched = matchSubsequentBytes(bits);
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
               _expectedByteCount = byte._byteCount;
               
               // Start the character value
               // using the bytes extract mask.
             
               _value = (
                  bits & byte._extractMask
               ).to_ulong();
               
               
               return true;
            }
               
         }
         
         return false;
      }
      
      bool matchSubsequentBytes(const bitset<8>& bits)
      {
         // Match subsequent bytes.
         
         // Check the byte is a subsequent
         // byte.
         UTF8Byte byte = UTF8Subsequent;
         if (byte.match(bits))
         {
            // Left shift the 6 usable bits
            // onto the character value.
            _value =
               (_value << 6) |
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

      BeeFishBString::Character character() {
         BeeFishBString::Character character(_value);
         return character;
      }      

      const BeeFishMisc::optional<bool> result() const {
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