#ifndef BEE_FISH_PARSER__CHAR_HPP
#define BEE_FISH_PARSER__CHAR_HPP

#include <iostream>
#include <string>
#include <bitset>

#include "ParserBase.hpp"

namespace BeeFishParser {

   class Char : public ParserBase
   {
   public:
      std::bitset<8> _bits;
      int _bitCount {0};
      char _char {-1};

      Char()
      {
      }

      virtual bool
      read(bool bit)
      {

         _bits[7 - _bitCount++] = bit;
         if (_bitCount == 8) {
            setResult(true);
         }
         return true;
      }


      virtual void setResult(
         Optional result
      )
      {
         if (result == true) {
            _char = _bits.to_ulong();
         }

         _result = result;
      }
   };


}

#endif
