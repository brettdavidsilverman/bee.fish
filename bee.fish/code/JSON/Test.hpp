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
   bool testVariables();
   
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
         
      success = success &&
         testVariables();

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
         false
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
      return testPattern(
         number,
         pattern ,
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
         testNumber("10b", true);

      success &=
         testNumber("10", true);
         
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

      auto pattern = _string;
      success = success &&
         testPattern(pattern, "\"\\u007A\"",true);

      success = success &&
         testValue(pattern.value(), "z");

      if (success) {
         auto pattern = _string;
         
         pattern.read("\"text\"");
      
         cerr << pattern._value << endl;
         
         success = success &&
            pattern._value == "text";
      }
         
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
         testPattern(Object(), "{", false);

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

   inline bool testVariables()
   {
      using namespace std;

      cout << "Testing variables" << endl;

      bool success = true;

      if (success)
      
      {
         Variable v = "Hello World";
         stringstream out;
         out << v;

         success = success &&
            testValue("\"Hello World\"", out.str());

      }

      if (success)
      {
         Variable v = "Hello\\World";
         stringstream out;
         out << v;

         success = success &&
            testValue("\"Hello\\\\World\"", out.str());
      }
      
      if (success)
      {
          
         Variable v =
            BeeFishScript::Number{260975};
            
         stringstream out;
         out << v;

         success = success &&
            testValue("260975", out.str());
      }
      
      if (success)
      {
         Variable v = BeeFishScript::Object{{"🐝","🌎"}};
         stringstream out;
         out << v;

         success = success &&
            testValue("{\n   \"🐝\": \"🌎\"\n}", out.str());
      }
      
      if (success)
      {
          cout << "\tParsing undefined: " << flush;
          JSON json;
          json.read("undefined");
          json.eof();
          Variable var = *(json._variable);
          
          success = success &&
             var == undefined;
             
          BeeFishMisc::outputSuccess(success);
          
      }
      
      if (success)
      {
          cout << "\tParsing null: " << flush;
          JSON json;
          json.read("null");
          json.eof();
          Variable var = *(json._variable);
          
          success = success &&
             var == nullptr;
             
          BeeFishMisc::outputSuccess(success);
          
      }
      
      if (success)
      {
          cout << "\tParsing Number: " << flush;
          JSON json;
          json.read("1234567");
          json.eof();
          Variable var = *(json._variable);
          Number num = var;
          success = success &&
             num == 1234567;
             
          BeeFishMisc::outputSuccess(success);
          
      }
      
      if (success)
      {
          cout << "\tParsing String: " << flush;
          JSON json;
          json.read("\"Bee Silverman\"");
          json.eof();
          Variable var = *(json._variable);
          std::string str = var;
          success = success &&
             str == "Bee Silverman";
             
          BeeFishMisc::outputSuccess(success);
          
      }
      
      if (success)
      {
          cout << "\tParsing Object: " << flush;
          JSON json;
          json.read("{\"name\": \"Bee\"}");
          json.eof();
          Variable var = *(json._variable);
          
          success = success &&
             var["name"] == "Bee";
          
             
          BeeFishMisc::outputSuccess(success);
          
      }
      
      if (success)
      {
          cout << "\tParsing Array: " << flush;
          JSON json;
          json.read("[\"name\", \"Bee\"]");
          json.eof();
          
          if (json._result != true) {
             cout << json.getErrorMessage() << endl;
             success = false;
          }
          else {
             Variable var = *(json._variable);
             ArrayPointer array = var;
             success = success &&
                array->size() == 2 &&
                var[Size(0)] == "name" &&
                var[Size(1)] == "Bee";
          }
             
          BeeFishMisc::outputSuccess(success);
          
      }
   
      if (success)
      {
          cout << "\tParsing Complex Object: " << flush;
          JSON json;
          json.read("{\"id\": \"007\", \"name\": {\"first\": \"Bee\", \"last\": \"Silverman\"}}");
          json.eof();
          if (json._result != true) {
             cout << json.getErrorMessage() << endl;
             success = false;
          }
          else {
             Variable var = *(json._variable);
             success = success &&
                (var["id"] == "007") &&
                (var["name"]["last"]
                   == "Silverman");
          }
             
          BeeFishMisc::outputSuccess(success);
          
      }
   
    
      BeeFishMisc::outputSuccess(success);

      return success;
   }
   
   
}

#endif