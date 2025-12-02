#ifndef BEE_FISH__HTTPS_TEST_HPP
#define BEE_FISH__HTTPS_TEST_HPP
#include <filesystem>
#include <bits/stdc++.h>
#include "../Miscellaneous/Miscellaneous.hpp"
#include "ParseURL.hpp"
#include "../test/test.h"
#include "../Id/Id.hpp"

namespace BeeFishHTTPS {

   using namespace BeeFishJSON;
   using namespace BeeFishMisc;
   using namespace BeeFishDatabase;
   using namespace BeeFishScript;
   using namespace BeeFishTest;
   using namespace BeeFishId;
   
   using namespace std::filesystem;

   inline bool testParseURL(const BString& origin);
   inline bool testURLQuery(Path userData, const BString& uri, const BString& queryStr, bool expectedResult, const BString& expectedValue = "");
   inline bool testLogon(const BString& origin);
   inline bool testAllFiles(const BString& origin, string directory);
   inline bool testFile(const BString& origin, string directory, string file, bool expect = true);
   
   inline bool test(int port) {
      cout << "Testing HTTPS" << endl;
      bool success = true;
      
      Id id;
      
      BString origin = HOST;
      origin += ":";
      origin += std::to_string(port);
      
    //  success = success &&
    //     testParseURL(origin);
         
      success = success &&
         testLogon(origin);
         
      success = success &&
         testAllFiles(origin, TEST_DIRECTORY);
      
      outputSuccess(success);
      
      return success;
   }

   bool testParseURL(const BString& origin)
   {
      cout << "\t" << "Testing URLParser " << flush;
      
      bool success = true;
      
      JSONDatabase db(origin);
      
      Path userData = db.origin()["test"];
      userData.clear();
      JSONPathParser json(userData[URLS]["name"]);
      std::stringstream stream("{\"name\":{\"first\":\"Bee\",\"last\":\"Silverman\", \"middle\": [\"David\", \"PK\"]}}");
      json.read(stream);
      json.eof();
      
      success &= (json.result() == true);
      outputSuccess(success);
      
      if (success)
      {
       
         success &= testURLQuery(userData, "name", "this.name.first", true, "\"Bee\"");
         success &= testURLQuery(userData, "name", "this[\"name\"][\"first\"]", true, "\"Bee\"");
         success &= testURLQuery(userData, "name", "this[\"name\"].first", true, "\"Bee\"");
         success &= testURLQuery(userData, "name", "this.name[\"first\"]", true, "\"Bee\"");
         success &= testURLQuery(userData, "name", "this.nameb", false, "Invalid property 'nameb'. Expected one of\r\n   'name'");
         success &= testURLQuery(userData, "name", "this.name.middle[0]", true, "\"David\"");
         success &= testURLQuery(userData, "name", "this.name.middle[2]", false, "Invalid index 2. Expected index range is [0, 1]");
         outputSuccess(success);
      }
      
      return success;
      
   }
   

   bool testURLQuery(Path userData, const BString& url, const BString& queryStr, bool expectedResult, const BString& expectedValue)
   {
      cout << "\t" << "Testing query " << queryStr << flush;
     
      BString error;

      optional<JSONPath> object = parseURL(
         userData,
         error,
         "127.0.0.1",
         "GET",
         url,
         queryStr
      );
      
      bool result = object.has_value();
      bool success = testValue("Expected result", expectedResult == result);
      
      std::stringstream stream;

      if (result)
         stream << object.value();
      else {
         stream << error;
      }
      
      BString value = stream.str();
      
      success = success &&
         testValue("\tExpected value",
            (value == expectedValue)
         );
         
      outputSuccess(success);

      if (!success)
      {
         cout << "Expected: " << endl << expectedValue << endl;
         cout << "Got: " << endl << value << endl;
      }
      
      return success;
   }
    
   std::optional<bool> testParser(Parser* parser) {

      parser->read(cin);
 
      parser->eof();

      cout << endl;

      return parser->result();

   }
   
   inline bool testAllFiles(const BString& origin, string directory)
   {
      cout << "Testing all files in " << directory << endl;

      bool success = true;
      
      vector<string> files;

      for (const auto & entry : directory_iterator(directory))
      {
         files.push_back(entry.path().filename());
      }

      sort(files.begin(), files.end());

      // Test via apache web server
      for (auto file : files) {
         if (success)
            success = testFile(origin, directory, file, true);
         else
            break;
      }

      outputSuccess(success);

      return success;
   }
   
   inline bool testLogon(const BString& origin)
   {
      cout << "Testing logon" << endl;

      
      stringstream stream;
      bool success = true;

      BeeFishScript::Object auth {
         {"method", "logon"},
         {"secret", "boo"}
      };
      
      BString str = escape(auth.str(false));
      
      stream
         << "curl "
         << origin  << "/authenticate"
         << " -c cookies -b cookies "
         << " -d \"" << str << "\""
         << " -s -k > /dev/null";


      string command = stream.str();

      cout << "\t" << command << endl;
      
      success = (system(command.c_str()) == 0);
      
      outputSuccess(success);
      
      return success;
   }
   
   inline bool testFile(const BString& origin, string directory, string file, bool expect)
   {
      cout << "\tTesting "
           << file
           << endl;

      string tempFile =
         TEMP_DIRECTORY;
         
      tempFile += "test.curl.txt";
      remove(tempFile);
      
      stringstream stream;
      bool success = true;

      stream << 
         "curl -X POST " <<
         origin << "/" <<
         file << " "
         << "-c cookies -b cookies " <<
         "-H \"Content-Type: application/json; charset=utf-8\" " <<
         "-H Expect: " <<
         "-T " << directory << "/" << file
         << " -s -k > /dev/null";


      string command = stream.str();

      success &= (system(command.c_str()) == 0);

      if (success && expect) {
          
         stringstream stream1;
         stream1
            << "curl "
            << origin  << "/" << file << " "
            << "-c cookies -b cookies "
            << " -s -k > "
            << tempFile;
            
         string command = stream1.str();
         success &= (system(command.c_str()) == 0);

         // Compare the files
         success = success &&
            compareFiles(tempFile, directory + "/" + file);

         
      }

      remove(tempFile);
      
      outputSuccess(success);
      return success;
   }
   
   
}

#endif