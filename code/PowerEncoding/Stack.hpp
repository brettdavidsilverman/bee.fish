#ifndef BEE_FISH__POWER_ENCODING__STACK_HPP
#define BEE_FISH__POWER_ENCODING__STACK_HPP

#include "PowerEncodingBase.hpp"

namespace BeeFishPowerEncoding {

   typedef std::vector<bool> Stack;

   class PowerEncodingStack :
      public PowerEncoding,
      public Stack
   {
   public:
      PowerEncodingStack() {
      }
     
      virtual bool peekBit()
      override
      {
         return (*this)[_index];
      }


   };

}

#endif
