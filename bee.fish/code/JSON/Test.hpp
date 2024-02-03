#ifndef BEE_FISH__JSON__TEST__HPP
#define BEE_FISH__JSON__TEST__HPP

#include <thread>
#include <stdlib.h>
#include "JSON.hpp"

namespace BeeFishJSON {
 
   using namespace BeeFishMisc;

   bool testBlankSpace();
   bool testConstants();
   bool testNumbers();
   bool testStrings();
   bool testArrays();
   bool testObjects();
   bool testJSON();
   bool testUnicode();

   inline bool test() {
      using namespace std;

      bool success = true;

      success = success &&
         testBlankSpace();

      success = success &&
         testConstants();

      success = success &&
         testNumbers();

      success = success &&
         testStrings();

      success = success &&
         testArrays();

      success = success &&
         testObjects();
         
      success = success &&
         testJSON();

      success = success &&
         testUnicode();

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
         blankSpace,
         " ",
         true
      );

      success &= testPattern(
         blankSpace,
         "\r",
         true
      );

      success &= testPattern(
         blankSpace,
         "\n",
         true
      );

      success &= testPattern(
         blankSpace,
         "\t",
         true
      );

      success &= testPattern(
         blankSpace,
         "z",
         false
      );

      success &= testPattern(
         blankSpace,
         " a",
         true	
      );

      success &= testPattern(
         blankSpace and
         Character("b"),
         " b",
         true
      );

      success &= testPattern(
         blankSpace and
         Character("c"),
         "c",
         false
      );

      success &= testPattern(
         blankSpace and
         Character("d") and
         blankSpace,
         "d",
         false
      );

      success &= testPattern(
         -blankSpace and
         Character("e"),
         "e",
         true
      );

      success &= testPattern(
         blankSpaces and
         Character("f"),
         "   f",
         true
      );

      BeeFishMisc::outputSuccess(success);

      return success;
      
   }

   inline bool testConstants()  {
      using namespace std;
      using namespace BeeFishParser;

      bool success = true;

      cout << "Testing Constants:" << endl;
     
      success &= testPattern(
         JSON(),
         "null",
         true
      );

      success &= testPattern(
         JSON(),
         "a",
         false
      );

      success &= testPattern(
         JSON(),
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

      success &= testPattern(
         JSON(),
         "true",
         true
      );

      success &= testPattern(
         JSON(),
         "false",
         true
      );

      success &= testPattern(
         JSON(),
         "undefined",
         true
      );

      BeeFishMisc::outputSuccess(success);

      return success;
      
   }

   static bool testNumber(
      const std::string& pattern,
      const std::optional<bool>&
          expected)
   {
      auto complex = number and
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
         testPattern(_string, "\"\"", true);
 
      success = success &&
         testPattern(_string, "\"\\\"\"", true);
 
      success = success &&
         testPattern(_string, "\"text\"", true);

      success = success &&
         testPattern(_string, "\"", nullopt);

      success = success &&
         testPattern(_string, "unquoted", false);

      BeeFishMisc::outputSuccess(success);

      return success;
      
   }

   inline bool testArrays()  {
      using namespace std;

      bool success = true;

      cout << "Testing Arrays:" << endl;

      success &=
         testPattern(array, "[]", true);
 
      success &=
         testPattern(array, "[\"a\"]", true);
 
      success &=
         testPattern(array, "[1,2]", true);

      success &=
         testPattern(array, "[1,null]", true);

      success &=
         testPattern(array, "[1,2,3]", true);

      success &=
         testPattern(array, "[[]]", true);

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
         testPattern(Object(), "{\"a\":{\"b\":{}}}", true);
         
         
      success &=
         testPattern(Object(), "{ \"a\" :{ \"b\" : { } } }", true);
         
      success &=
         testPattern(Object(), "{", nullopt);

      success &=
         testPattern(Object(), "}", false);
         
         
      BeeFishMisc::outputSuccess(success);

      return success;
      
   }
   
   inline bool testJSON() {
       
      bool success = true;
      cout << "Testing JSON" << endl;
      
      success &=
         testPattern(JSON(), "{ \"a\" :{ \"b\" : { } } }", true);
         
      return success;
   }
   
   inline bool testUnicode()
   {
      using namespace std;

      bool success = true;

      cout << "Testing unicode" << endl;

      string captured;

      success &=
         testPattern(Capture(JSON(), captured), "{\"🐝\":\"🌎\"}", true);

      success &=
         testValue("{\"🐝\":\"🌎\"}", captured);

      BeeFishMisc::outputSuccess(success);

      return success;
      
   }

   
   
}

#endif