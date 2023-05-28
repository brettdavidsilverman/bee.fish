#ifndef BEE_FISH_PARSER__CHAR_HPP
#define BEE_FISH_PARSER__CHAR_HPP

#include <iostream>
#include <string>
#include <bitset>

#include "ParserBase.hpp"

namespace BeeFishParser {

   class Char : public Parser
   {
   public:
      std::bitset<8> _bits;
      int _bitCount {0};
      char _char {-1};

      Char()
      {
      }

      Char(char c) : _char(c)
      {
      }

      virtual bool
      read(bool bit)
      {
         //std::cerr << (bit ? 1 : 0) << std::flush;

         if (_bitCount >= 8)
            return false;

         _bits[7 - _bitCount] = bit;
         if (++_bitCount == 8) {
            setResult(true);
         }
         return true;
      }


      virtual void setResult(
         std::optional<bool> result
      ) override
      {
         if (result == true) {
            _char = _bits.to_ulong();
         }

         Parser::setResult(result);

      }

      virtual Parser* copy() const {
         return new Char(_char);
      }
      
      virtual bool read(char c) {
         throw std::logic_error("Should not reach here");
      }
   };


}

#endif
