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
   protected:
      long int _count = 0;
      long int _index = 0;

   public:
      virtual void writeBit(bool bit)
      {
         if (bit)
            ++_count;
         else
            --_count;

          ++_index;
      }

      virtual bool readBit()
      {
         bool bit = peekBit();

         ++_index;

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

      friend PowerEncoding& operator<<(
          PowerEncoding &stream,
          const char* string)
      {
         stream.writeBit(1);

         for (const char* c = string;
              *c != 0;
              ++c)
         {
            stream << *c;
         }

         stream.writeBit(0);

         return stream;
      }

      friend PowerEncoding& operator<<(
          PowerEncoding &stream,
          const std::string& string)
      {

         stream.writeBit(1);

         for (auto character : string)
         {
            stream << character;
         }

         stream.writeBit(0);

         return stream;
      }

   };
   
}

#endif