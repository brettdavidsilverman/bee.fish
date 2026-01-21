#ifndef BEE_FISH__POWER_ENCODING_BASE_H
#define BEE_FISH__POWER_ENCODING_BASE_H

#include <iostream>
#include <string>
#include <vector>
#include <tgmath.h>
#include <cassert>

using namespace std;

namespace BeeFishPowerEncoding
{
   class PowerEncoding
   {
   protected:
      long int _count {0};

   public:
      PowerEncoding()
      {
      }

      template<typename T>
      void write(const T& value)
      {
         static const T zero {};

         if (value == zero)
            writeBit(0);
         else
         {
            auto _power     =
                 power(value);
               
            auto _remainder =
                 remainder(value, _power);

            writeBit(1);
            write(_power);
            write(_remainder);
         }
      }
      
      template<typename T>
      void read(T& value)
      {
         bool bit = readBit();

         if (bit == 0)
         {
            value = T();
            return;
         }
            
         unsigned long _power;
         read(_power);
         
         unsigned long _remainder;
         read(_remainder);
         
         value = exp2(_power) + _remainder;
         
      }

      template<typename T>
      unsigned long power(T value)
      {
         unsigned long power =
            floor(log2(value));

         return power;
      }
      
      template<typename T>
      unsigned long remainder
      (
         T value,
         unsigned long power
      )
      {
         unsigned long remainder =
            value - exp2(power);

         return remainder;
      }

      long int count() const {
         return _count;
      }

      virtual void reset() {
         resetCount();
      }
      
      virtual void resetCount() {
         _count = 0;
      }

   public:
       /*
      template<typename T>
      friend PowerEncoding& operator << (PowerEncoding& out, const T& value) {
         out.write(value);
         return out;
      }
      
      template<typename T>
      friend PowerEncoding& operator >> (PowerEncoding& in, T& value) {
         in.read(value);
         return in;
      }
      */
      virtual void writeBit(bool bit)
      {
         if (bit)
            ++_count;
else if (_count > 0)
            --_count;

      }

      virtual bool readBit()
      {
         bool bit = peekBit();

         if (bit)
            ++_count;
         else if (_count > 0)
            --_count;

         return bit;
      }
   
      virtual bool peekBit() const
      {
         throw logic_error("peekBit");
         assert(false);
      }

   };
   
}

#endif
