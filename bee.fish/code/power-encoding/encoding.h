#ifndef BEE_FISH_POWER_ENCODING__ENCODING_H
#define BEE_FISH_POWER_ENCODING__ENCODING_H

#include <iostream>
#include "power-encoding-base.h"

using namespace std;

namespace BeeFishPowerEncoding
{
    
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
      PowerEncoding &input,
      char& character
   )
   {
      // read one
      input.readBit();

      uint8_t read;
      input.read(read);

      character = (char)read;

      return input;
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
      string.clear();
      
      char character;
      
      // read one
      assert(input.readBit() == 1);
      
      while (input.peekBit())
      {
         input >> character;
         
         string.push_back(character);
      }

      // read zero
      assert(input.readBit() == 0);

      return input;
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
      // read one
      assert(input.readBit() == 1);

      input.read(value);

      return input;
   }

   class EncodeToStream : public PowerEncoding
   {
   protected:
      istream& _in;
      ostream& _out;
   
   public:
      EncodeToStream(istream& in, ostream& out) :
         _in(in),
         _out(out)
      {
      }
      
      virtual void writeBit(bool bit)
      {
#ifdef DEBUG
         cerr << 'w' << (bit ? '1' : '0');
#endif
         _out << ( bit ? '1' : '0' );

         PowerEncoding::writeBit(bit);
      }
   
      virtual bool readBit()
      {
         if (_in.eof())
            return 0;

         PowerEncoding::readBit();

         char bit; 
         _in >> bit;
      
#ifdef DEBUG
         cerr << 'r' << bit;
#endif

         bool b = ( bit != '0' );
      
         return b;
      }
   
      virtual bool peekBit() const
      {
         return _in.peek() != '0';
      }
   
   };
   



}

#endif