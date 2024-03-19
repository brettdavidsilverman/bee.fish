#ifndef BEE_FISH_POWER_ENCODING__ENCODING
#define BEE_FISH_POWER_ENCODING__ENCODING

#include <iostream>
#include "power-encoding.h"

using namespace std;

namespace BeeFishPowerEncoding
{

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
   
      virtual bool peekBit()
      {
         return _in.peek() != '0';
      }
   
   };
   



}

#endif