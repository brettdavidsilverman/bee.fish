#ifndef BEE_FISH_B_SCRIPT__TEST_H
#define BEE_FISH_B_SCRIPT__TEST_H

#include "../test/test.h"
#include "../parser/test.h"
#include "../json/json.h"
#include "b-script.h"

using namespace BeeFishParser;

namespace BeeFishBScript
{
   
   inline bool test();

   inline bool parse(const BString& string, Variable& variable);

   inline bool testIntrinsics();
   inline bool testNumbers(); 
   inline bool testStrings();
   inline bool testArrays();
   inline bool testObjects();
#ifdef SERVER
   //inline bool testJSONStreams();
#endif

   inline bool test()
   {
      
      bool ok = true;

      ok &= testIntrinsics();
      ok &= testNumbers();
      ok &= testStrings();
      ok &= testObjects();
      ok &= testArrays();
#ifdef SERVER
      //ok &= testJSONStreams();
#endif

      if (ok)
         cout << "SUCCESS" << endl;
      else
         cout << "FAIL" << endl;
         
      return ok;
   }
   
   inline bool parse(const BString& string, Variable& variable) {
      
      BeeFishJSON::JSON json;
      BeeFishBScript::BScriptParser parser(json);
      parser.read(string);
      
      if (parser.result() == true) {
         variable = parser.value();
         return true;
      }

      return false;
   }


   inline bool testIntrinsics()
   {
      cout << "Intrinsics" << endl;

      bool ok = true;

      BeeFishBScript::Variable variable = Variable::Undefined();
      
      std::stringstream streamUndefined;
      streamUndefined << variable;
      ok &= testResult("Variable undefined to string", (streamUndefined.str() == "undefined"));

      variable = true;
      std::stringstream streamTrue;
      streamTrue << variable;
      ok &= testResult("Boolean true to string", (streamTrue.str() == "true"));
      
      variable = false;
      std::stringstream streamFalse;
      streamFalse << variable;
      ok &= testResult("Boolean false to string", (streamFalse.str() == "false"));

      variable = nullptr;
      std::stringstream streamNull;
      streamNull << variable;
      ok &= testResult("Null to string", (streamNull.str() == "null"));

      Variable var;
      ok &= parse("undefined", var);
      std::cout << "VARIABLE: " << var << std::endl;
      ok &= testResult("Parse undefined", var._type == BeeFishJSON::UNDEFINED);

      ok &= parse("true", variable);
      ok &= testResult("Parse true", variable == true);

      ok &= parse("false", variable);
      ok &= testResult("Parse false", variable == false);

      cout << endl;
      
      return ok;
   }
  
   inline bool testNumbers()
   {
      cout << "Numbers" << endl;
      
      bool ok = true;

      BeeFishBScript::Variable variable;

      ok &= parse("4.5 ", variable);
      ok &= testResult("Parse decimal number", variable == 4.5);

      ok &= parse("-4.5 ", variable);
      ok &= testResult("Parse negative decimal number", variable == -4.5);

      ok &= parse("10E2 ", variable);
      ok &= testResult("Parse exponent number", variable == 1000.0);


      double value = 1.0 / 0.0;
      variable = value;
      ok &= testResult("Not a number", !isnormal((Number)variable));

      std::stringstream stream;
      stream << variable;
      BString string = stream.str();
      ok &= testResult("Not a number as string", string == "\"NaN\"");

      cout << endl;
      
      return ok;
   }    

   inline bool testStrings()
   {
      cout << "Strings" << endl;
      
      bool ok = true;
  
      BeeFishBScript::Variable variable = "hello world";
      
      std::stringstream stream;
      stream << variable;
      ok &= testResult("String variable hello world", (stream.str() == "\"hello world\""));

      ok &= parse("\"Goodbye world\"", variable);
      ok &= testResult("Parse string", variable == "Goodbye world");
      cout << endl;
      
      BeeFishBScript::Variable variable2;
      
      BeeFishJSON::JSON json;
      BeeFishBScript::BScriptParser parser(json);

      parser.read("\"😀\"");
      variable2 = parser.json();
      ok &= testResult("Parse string 😀", variable2 == "😀");

      BString jsonstring = parser.json().str();
      ok &= testResult("To string 😀", jsonstring == "\"😀\"");

      cout << endl;

      return ok;
   }

   inline bool testArrays()
   {
      cout << "Arrays" << endl;
      
      bool ok = true;

      BeeFishBScript::Variable variable = BeeFishBScript::Array({1,2,3});
      
      std::stringstream stream;
      stream << variable;
      ok &= testResult("Array to string", (stream.str() == "[1, 2, 3]"));

      ok &= parse("[1,2,3]", variable);
      std::shared_ptr<Array> array = variable;
      ok &= testResult("Array size", (array->size() == 3));
      BeeFishBScript::Number& number = (*array)[1];
      ok &= testResult("Array element", (number == 2));
      
      cout << endl;
      
      return ok;
      
   }


   inline bool testObjects()
   {
      cout << "Objects" << endl;
      
      bool ok = true;

      BeeFishBScript::Variable variable = BeeFishBScript::Object{
         {"name", "Brett"}
      };
      
      std::stringstream stream;
      stream << variable;
      const std::string string = 
         "{\n"
         "   \"name\": \"Brett\"\n"
         "}";
         
      ok &= testResult("Object variable to string", (string == stream.str()));

      ok &= parse("{\"name\": \"Silverman\"}", variable);
      BeeFishBScript::ObjectPointer object = variable;
      ok &= testResult("Parsed object", (BeeFishBScript::String&)((*object)["name"]) == "Silverman");

      ok &= parse("{\"name\": {\"first\": \"Brett\", \"last\": \"Silverman\"}}", variable);
      object = variable;
      Variable& firstName = (*object)["name"]["first"];
      ok &= testResult("Nested object", firstName == "Brett");

      cout << endl;
      
      return ok;
      
   }
   /*
#ifdef SERVER
   inline bool testJSONStreamResult(
      const char* label,
      Path path,
      const BString& json,
      const Variable& variable
   )
   {

      Path bookmark = path;

      JSONInStream stream(path);
      
      stream << json;
      
      bool ok = true;

      ok &= testResult(
         std::string(label) + " input",
         stream.result() == true
      );

      JSONOutStream outputstream(bookmark);

      Variable var;
      outputstream >> var;

      if (var._type != BeeFishJSON::OBJECT) {
         ok &= testResult(
            std::string(label) + " output",
            var == variable
         );
      }

      return ok;
   }
   
   inline bool testJSONStreams() {
   
      cout << "JSON In Stream" << endl;
      
      using namespace BeeFishDatabase;
      
      bool ok = true;
      
      Database db("json-in-stream.db");
      
      Path root(db);
      
      Path path = root;
      
      ok &= testJSONStreamResult(
         "Undefined",
         path,
         "undefined",
         undefined
      );
      
      path = root;
      
      ok &= testJSONStreamResult(
         "Null",
         path,
         "null",
         nullptr
      );
      
      path = root;
      
      ok &= testJSONStreamResult(
         "Number",
         path,
         "123.45 ",
         123.45
      );
      
      path = root;
      
      ok &= testJSONStreamResult(
         "String",
         path,
         "\"Hello World\"",
         "Hello World"
      );
      
      path = root;
      
      ok &= testJSONStreamResult(
         "Empty Object",
         path,
         "{}",
         nullptr
      );
      
      ok &= testJSONStreamResult(
         "Object",
         path,
         "{\"first name\": \"Brett\", "
          "\"last name\" : \"Silverman\"}",
         nullptr
      );
      
      
      remove("json-in-stream.db");
      
      cout << endl;
      
      return ok;
   }
#endif

      */
}

#endif
