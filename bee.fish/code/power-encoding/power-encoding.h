#ifndef BEE_FISH__POWER_ENCODING_H
#define BEE_FISH__POWER_ENCODING_H

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
   private:
      long int _count = 0;
            
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
   
      virtual bool peekBit()
      {
         return 0;
      }

      
   public:
      PowerEncoding()
      {
      }
     
      template<typename T>
      PowerEncoding& operator <<
      (const T& value)
      {
      
         if (value == 0)
            writeBit(0);
         else
         {
            writeBit(1);
            
            auto _power     =
                 power(value);
               
            auto _remainder =
                 remainder(value, _power);

            *this << _power;
            *this << _remainder;
         }
         return *this;
      }
     
      template<typename T>
      PowerEncoding& operator >>
      (T& value)
      {
         bool bit = readBit();
         if (bit == 0)
         {
            value = T();
            return *this;
         }
            
         unsigned long _power;
         (*this) >> _power;
         
         unsigned long _remainder;
         (*this) >> _remainder;
         
         value = exp2(_power) + _remainder;
         
         return *this;
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

      long int count() {
         return _count;
      }

      virtual void reset() {
         resetCount();
      }

      virtual void resetCount() {
         _count = 0;

      }
   };
   
}

#endif
