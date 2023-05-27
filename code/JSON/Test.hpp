#ifndef BEE_FISH__JSON__TEST__HPP
#define BEE_FISH__JSON__TEST__HPP

#include <thread>
#include <stdlib.h>
#include "JSON.hpp"

namespace BeeFishJSON {
 
   using namespace BeeFishMisc;

   bool testBlankSpace();
   bool testNull();
   bool testNumbers();
   bool testStrings();
   bool testArrays();
   bool testObjects();
   

   inline bool test() {
      using namespace std;

      bool success = true;

      success = success &&
         testBlankSpace();

      success = success &&
         testNull();

      success = success &&
         testNumbers();

      success = success &&
         testStrings();

      success = success &&
         testArrays();

      success = success &&
         testObjects();

      if (success)
         cout << "JSON tests pass" << endl;
      else
         cout << "JSON tests failed" << endl;

      return success;
   }

   inline bool testBlankSpace()  {
      using namespace std;
      using namespace BeeFishParser;

      bool success = true;

      cout << "Testing Blank Space:" << endl;
     
      success &= testPattern(
         BlankSpace(),
         " ",
         nullopt
      );

      success &= testPattern(
         BlankSpace(),
         "\r",
         nullopt
      );

      success &= testPattern(
         BlankSpace(),
         "\n",
         nullopt
      );

      success &= testPattern(
         BlankSpace(),
         "\t",
         nullopt
      );

      success &= testPattern(
         BlankSpace(),
         "a",
         false
      );

      success &= testPattern(
         BlankSpace(),
         " a",
         false
      );

      success &= testPattern(
         BlankSpace() and
         Character("a"),
         " a",
         true
      );

      success &= testPattern(
         BlankSpace() and
         Character("a"),
         "a",
         true
      );

      success &= testPattern(
         BlankSpace() and
         Character("a") and
         BlankSpace(),
         "a",
         nullopt
      );

      BeeFishMisc::outputSuccess(success);

      return success;
      
   }

   inline bool testNull()  {
      using namespace std;
      using namespace BeeFishParser;

      bool success = true;

      cout << "Testing Null:" << endl;
     
      success &= testPattern(
         Null(),
         "null",
         true
      );

      success &= testPattern(
         Null(),
         "a",
         false
      );

      success &= testPattern(
         Null(),
         "nulL",
         false
      );

      success &= testPattern(
         JSON(),
         "nul",
         nullopt
      );

      success &= testPattern(
         JSON(),
         "anull",
         false
      );

      BeeFishMisc::outputSuccess(success);

      return success;
      
   }

   static bool testNumber(
      const std::string& pattern,
      const std::optional<bool>&
          expected)
   {
      auto complex = Number() and
         BeeFishParser::Word("\r\n");

      return testPattern(
         complex,
         pattern + "\r\n",
         expected
      );
   }

   inline bool testNumbers()  {
      using namespace std;

      bool success = true;

      cout << "Testing Numbers:" << endl;

      success &=
         testNumber("1", true);
 
      success &=
         testNumber("-1", true);
 
      success &=
         testNumber("-1e+1", true);

      success &=
         testNumber("-1.2e-10", true);

      success &=
         testNumber("10b", false);


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
         testPattern(String(), "\"\\\"\"", true);
 
      success = success &&
         testPattern(String(), "\"text\"", true);

      success = success &&
         testPattern(String(), "\"", nullopt);

      success = success &&
         testPattern(String(), "unquoted", false);

      BeeFishMisc::outputSuccess(success);

      return success;
      
   }

   inline bool testArrays()  {
      using namespace std;

      bool success = true;

      cout << "Testing Arrays:" << endl;

      success &=
         testPattern(Array(), "[]", true);
 
      success &=
         testPattern(Array(), "[\"a\"]", true);
 
      success &=
         testPattern(Array(), "[,]", false);

      success &=
         testPattern(Array(), "[1,null]", true);

      success &=
         testPattern(Array(), "[1,2,3]", true);

      success &=
         testPattern(Array(), "[[]]", true);

      BeeFishMisc::outputSuccess(success);

      return success;
      
   }

   inline bool testObjects()  {
      using namespace std;

      bool success = true;

      cout << "Testing Objects:" << endl;

      success &=
         testPattern(Object(), "{}", true);
 
      success &=
         testPattern(Object(), "{\"a\":1}", true);
 
      success &=
         testPattern(Object(), "{\"a\":1,\"b\":2}", true);

      success &=
         testPattern(Object(), "{\"c\":{}}", true);

      success &=
         testPattern(Object(), " { \"c\" : { } }", true);

      success &=
         testPattern(Object(), "{", nullopt);

      BeeFishMisc::outputSuccess(success);

      return success;
      
   }

   
   
}

#endif