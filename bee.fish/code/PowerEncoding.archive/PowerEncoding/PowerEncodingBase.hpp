#ifndef BEE_FISH__POWER_ENCODING_BASE_HPP
#define BEE_FISH__POWER_ENCODING_BASE_HPP

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
   public:
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

      virtual long int count() const {
         return _count;
      }

      virtual void reset() {
         _count = 0;
      }

   public:
      virtual void writeBit(bool bit)
      {
         if (bit)
            ++_count;
         else
            --_count;

      }

      virtual bool readBit()
      {
         bool bit = peekBit();

         if (bit)
            ++_count;
         else
            --_count;

         return bit;
      }
   
      virtual bool peekBit() const
      {
         assert(false);
      }

      
   };
   
}

#endif
