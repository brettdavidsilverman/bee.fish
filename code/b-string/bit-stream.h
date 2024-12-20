#ifndef BEE_FISH_B_STRING__BIT_STREAM_H
#define BEE_FISH_B_STRING__BIT_STREAM_H

#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <ctype.h>
#include <bitset>
#include <cassert>

#include "../power-encoding/power-encoding.h"
#include "b-string.h"
#include "char.h"

namespace BeeFishBString {

   // A byte aligned stream of bits
   // using a vector<bool> as storage
   // and PowerEncoding as the encoding
   // mechanism

   // This defines the readBit and writeBit
   // functions of PowerEncoding, to append
   // a bit to the vector.
   class BitStream :
      public PowerEncoding,
      public vector<bool>
   {
   private:
      vector<bool>::const_iterator _it;
      
   public:

      BitStream()
      {
         _it = cend();
      }
      
      BitStream(const BitStream& bitStream) :
         vector<bool>(bitStream)
      {
          _it = cbegin();
      }
      
      BitStream(const BString& source) {
         for (auto character : source) {
            (*this) << character;
         }
      }

      BitStream(const std::string& data)
      {
         // [0,1,2,3,4,5,6,7]
         const Byte* _data = (const Byte*)data.data();
         const size_t _size = data.size();

         reserve(_size * 8);

         ssize_t count = 0;

         for (size_t byteIndex = 0; byteIndex < _size; ++byteIndex) {

            Byte byte = _data[byteIndex];

            bitset<8> bits = byte;

            for ( size_t bitIndex = 0;
                  bitIndex < 8;
                  ++bitIndex )
            {
               bool bit = bits[7 - bitIndex];

               if (count >= 0) {

                  if (bit)
                     ++count;
                  else
                     --count;

                  if (count >= 0)
                     push_back(bit);

               }

            }
         }
        
         _it = cbegin();
    
      }

      static BitStream fromData(const std::string& data)
      {
         return BitStream(data);
      }
      
      std::string toData() const
      {
         std::vector<Byte> bytes;
         int count = 0;
         int bitCount = 0;
         bitset<8> bits = 0;
         
         for (bool bit : *this)
         {

            bits[7 - bitCount] = bit;
            if (++bitCount == 8)
            {
               Byte byte =
                  bits.to_ulong();
               bytes.push_back(byte);
               bits = 0;
               bitCount = 0;
            }
            if (bit)
               ++count;
            else
               --count;
         }
         
         if (bitCount > 0)
         {
            Byte byte =
               bits.to_ulong();
            bytes.push_back(byte);
         }

         std::string copy((const char*)bytes.data(), bytes.size());

         return copy;
      }
      
      virtual void writeBit(bool bit)
      {
         push_back(bit);

         PowerEncoding::writeBit(bit);
      }
      
      virtual bool readBit()
      {
      

         bool bit = PowerEncoding::readBit();

         if (_it != cend())
            ++_it;
         
         return bit;
         
      }
      
      virtual bool peekBit() const
      override
      {
         if (_it == cend())
            return 0;
            
         bool bit = *_it;
         
         return bit;
      }

      virtual void reset()
      {
         _it = cbegin();
         PowerEncoding::reset();
      }
      
      friend ostream& operator << (ostream& out, const BitStream& bitStream) {
         bitStream.write(out);
         return out;
      }

      virtual void write(ostream& out) const {
         for (auto bit : (*this)) {
            out << (bit ? '1' : '0');
         }
      }

      BString bstr() {
         BString string;
         for (auto bit : (*this)) {
            string.push_back(bit ? '1' : '0');
         }
         return string;
      }
      
   };
   
}

#endif


