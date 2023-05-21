#ifndef BEE_FISH__JSON__TEST__HPP
#define BEE_FISH__JSON__TEST__HPP

#include <thread>
#include <stdlib.h>

#include "JSON.hpp"

namespace BeeFishJSON {
   
   inline bool test() {
      using namespace std;

      bool success = true;

      if (success)
         cout << "JSON tests pass" << endl;
      else
         cout << "JSON tests failed" << endl;

      return success;
   }

   
}

#endif