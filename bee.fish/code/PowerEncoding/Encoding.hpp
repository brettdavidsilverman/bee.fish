#ifndef BEE_FISH__POWER_ENCODING__ENCODING_HPP
#define BEE_FISH__POWER_ENCODING__ENCODING_HPP

#include <iostream>
#include "PowerEncodingBase.hpp"

using namespace std;
using namespace BeeFishPowerEncoding;

namespace BeeFishPowerEncoding{

   PowerEncoding& operator <<
   (
      PowerEncoding &output,
      const char& character
   )
   {
      output.writeBit(1);

      // Write as unsigned because
      // PowerEncoding only handles
      // positive integers
      output.write((uint8_t)character);

      return output;
   }

   PowerEncoding& operator >>
   (
      PowerEncoding &output,
      char& character
   )
   {
      assert(output.readBit() == 1);

      uint8_t read;
      output.read(read);

      character = (char)read;

      return output;
   }

   PowerEncoding& operator <<
   (
      PowerEncoding &output,
      const std::string& string
   )
   {
      output.writeBit(1);

      for (const int character : string)
      {
         output << character;
      }

      output.writeBit(0);

      return output;
   }
   
   PowerEncoding& operator >>
   (
      PowerEncoding &input,
      std::string& string
   )
   {

      char character;
      assert(input.readBit() == 1);

      while (input.peekBit())
      {
         input >> character;
         string += character;
      }

      assert(input.readBit() == 0);

      return input;
   }

   PowerEncoding& operator <<
   (
      PowerEncoding& output,
      const char* string
   )
   {

      return output << std::string(string);
   }


   template<typename T>
   PowerEncoding& operator <<
   (
      PowerEncoding& output,
      const T& value
   )
   {
      output.writeBit(1);
      output.write(value);

      return output;
   }

   template<typename T>
   PowerEncoding& operator >>
   (
      PowerEncoding& input,
      T& value
   )
   {
      assert(input.readBit() == 1);

      input.read(value);

      return input;
   }

   
   class EncodeToStream : public PowerEncoding
   {
   protected:
      istream& _input;
      ostream& _output;
   
   public:
      EncodeToStream(istream& input, ostream& output) :
         _input(input),
         _output(output)
      {
      }
      
      virtual void writeBit(bool bit)
      {
         PowerEncoding::writeBit(bit);
         _output << ( bit ? '1' : '0' );
      }
   
      virtual bool readBit()
      override
      {
         assert(!_input.eof());

         PowerEncoding::readBit();

         char character; 
         _input >> character;

         bool bit = ( character == '1' );
      
         return bit;
      }
   
      virtual bool peekBit() const
      override
      {
         return (_input.peek() == '1');
      }

   };

   
}
#endif