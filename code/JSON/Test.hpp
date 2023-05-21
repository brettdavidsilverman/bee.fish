#ifndef BEE_FISH__JSON__TEST__HPP
#define BEE_FISH__JSON__TEST__HPP

#include <thread>
#include <stdlib.h>

#include "JSON.hpp"

namespace BeeFishJSON {
 
   bool testNumbers();
   bool testNumber(const std::string&, bool expected);

   inline bool test() {
      using namespace std;

      bool success = true;

      success &= testNumbers();

      if (success)
         cout << "JSON tests pass" << endl;
      else
         cout << "JSON tests failed" << endl;

      return success;
   }

   inline bool testNumbers()  {
      using namespace std;

      bool success = true;

      cout << "Testing Numbers:" << endl;

      std::string string;
         
      success = success &&
         testNumber("-1.2e-10", true);
 
      BeeFishMisc::outputSuccess(success);

      return success;
      
   }

   inline bool testNumber(
      const std::string& string,
      bool expected
   )
   {
      bool success = true;

      cout << "\t" << string << ": " << flush;
      auto number = Number();
      number.read(string + "\r\n");
      success = success &&
         number.result() == expected;

      BeeFishMisc::outputSuccess(success);

      return success;

   }

   
}

#endif