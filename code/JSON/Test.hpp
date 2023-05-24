#ifndef BEE_FISH__JSON__TEST__HPP
#define BEE_FISH__JSON__TEST__HPP

#include <thread>
#include <stdlib.h>
#include "JSON.hpp"

namespace BeeFishJSON {
 
   using namespace BeeFishMisc;

   bool testIntegers();
   bool testNumbers();
   bool testStrings();

   inline bool test() {
      using namespace std;

      bool success = true;

      success &= testIntegers();
      success &= testNumbers();
      success &= testStrings();

      if (success)
         cout << "JSON tests pass" << endl;
      else
         cout << "JSON tests failed" << endl;

      return success;
   }

   inline bool testIntegers()  {
      using namespace std;

      auto Integers = []() {
         return Repeat(Range("0", "9"));
      };

      bool success = true;

      cout << "Testing Integers:" << endl;

      success = success &&
         testPattern(Integers(), "123", nullopt);
 
      success = success &&
         testPattern(Integers(), "a", false);
 
      success = success &&
         testPattern(Integers(), "10b", true);

      BeeFishMisc::outputSuccess(success);

      return success;
      
   }


   static bool testNumber(
      const std::string& pattern,
      const std::optional<bool>&
          expected)
   {
      auto complex = Number();// and
        // BeeFishParser::Word("\r\n");

      return testPattern(
         complex,
         pattern,
         expected
      );
   }

   inline bool testNumbers()  {
      using namespace std;

      bool success = true;

      cout << "Testing Numbers:" << endl;

      success &=
         testNumber("1", nullopt);
 
      success &=
         testNumber("-1", nullopt);
 
      success &=
         testNumber("-1e+1", nullopt);

      success &=
         testNumber("-1.2e-10", nullopt);

      success &=
         testNumber("10b", true);


      BeeFishMisc::outputSuccess(success);

      return success;
      
   }

   inline bool testStrings()  {
      using namespace std;

      bool success = true;

      cout << "Testing Strings:" << endl;

      success = success &&
         testPattern(String(), "\"\"", true);
 
      success = success &&
         testPattern(String(), "\"", false);
 
      success = success &&
         testPattern(String(), "\"text\"", true);

      success = success &&
         testPattern(String(), "unquoted", false);

      BeeFishMisc::outputSuccess(success);

      return success;
      
   }

   
   
}

#endif