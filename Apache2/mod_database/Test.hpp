#ifndef BEE_FISH__APACHE2__TEST_HPP
#define BEE_FISH__APACHE2__TEST_HPP
#include <filesystem>
#include <bits/stdc++.h>
#include "Miscellaneous/Miscellaneous.hpp"
#include "ParseURI.hpp"
#include "test/test.h"
#include "Id/Id.hpp"

namespace BeeFishApache2 {

   using namespace BeeFishJSON;
   using namespace BeeFishMisc;
   using namespace BeeFishDatabase;
   using namespace BeeFishScript;
   using namespace BeeFishTest;
   using namespace BeeFishId;
   
   using namespace std::filesystem;

   inline bool testParseURI();
   inline bool testURIQuery(Database& database, const BString& uri, const BString& queryStr, bool expectedResult, const BString& expectedValue = "");
   
   inline bool testAllFiles(string url, string directory);
   inline bool testFile(string url, string directory, string file, bool expect = true);
   
   inline bool test() {
      cout << "Testing Apache2" << endl;
      bool success = true;
      
      Id id;
      
      string url = HOST;
      
         //BString(HOST) + BString("/") + id.key();
     
      success = success &&
         testParseURI();
         
      success = success &&
         testAllFiles(url, TEST_DIRECTORY);
      
      outputSuccess(success);
      
      return success;
   }

   bool testParseURI()
   {
      cout << "\t" << "Testing URIParser " << flush;
      
      bool success = true;
      
      Database db;
      Path start = Path(db)[HOST][URLS]["name"];
      start.clear();
      JSONPathParser json(start);
      std::stringstream stream("{\"name\":{\"first\":\"Bee\",\"last\":\"Silverman\", \"middle\": [\"David\", \"PK\"]}}");
      json.read(stream);
      json.eof();
      
      success &= (json.result() == true);
      outputSuccess(success);
      
      if (success)
      {
       
         success &= testURIQuery(db, "name", "this.name.first", true, "\"Bee\"");
         success &= testURIQuery(db, "name", "this[\"name\"][\"first\"]", true, "\"Bee\"");
         success &= testURIQuery(db, "name", "this[\"name\"].first", true, "\"Bee\"");
         success &= testURIQuery(db, "name", "this.name[\"first\"]", true, "\"Bee\"");
         success &= testURIQuery(db, "name", "this.nameb", false, "Invalid property \"nameb\". Expected one of\r\n   \"name\"");
         success &= testURIQuery(db, "name", "this.name.middle[0]", true, "\"David\"");
         success &= testURIQuery(db, "name", "this.name.middle[2]", false, "Invalid index 2. Expected index range is [0, 1]");
         outputSuccess(success);
      }
      
      return success;
      
   }
   

   bool testURIQuery(Database& database, const BString& uri, const BString& queryStr, bool expectedResult, const BString& expectedValue)
   {
      cout << "\t" << "Testing query " << queryStr << flush;
     
      BString error;

      optional<JSONPath> object = parseURI(
         database,
         error,
         "127.0.0.1",
         uri,
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
   
   inline bool testAllFiles(string url, string directory)
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
            success = testFile(url, directory, file, true);
         else
            break;
      }

      outputSuccess(success);

      return success;
   }
   
   inline bool testFile(string url, string directory, string file, bool expect)
   {
      cout << "\tTesting "
           << url << "/" << file
           << endl;

      string tempFile =
         TEMP_DIRECTORY;
         
      tempFile += "test.curl.txt";
      remove(tempFile);
      
      stringstream stream;
      bool success = true;

      stream << 
         "curl -X POST " <<
         url  << "/" <<
         file << " "
         "-H \"Content-Type: application/json; charset=utf-8\" " <<
         "-H Expect: " <<
         "-T " << directory << "/" << file
         << " -s -k | grep Success > /dev/null ";


      string command = stream.str();

      success &= (system(command.c_str()) == 0);

      if (success && expect) {
          
         stringstream stream1;
         stream1
            << "curl "
            << url  << "/" << file << " "
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