#ifndef BEE_FISH__DBSERVER__TEST_HPP
#define BEE_FISH__DBSERVER__TEST_HPP
#include <filesystem>
#include <bits/stdc++.h>
#include "../Miscellaneous/Miscellaneous.hpp"
#include "../Test/Test.hpp"
#include "DBServer.hpp"
#include "JSONPath.hpp"

#define TEMP_FILENAME "/var/tmp/bee.fish.tmp"

namespace BeeFish
{
   using namespace BeeFishJSON;
   using namespace BeeFishMisc;
   using namespace BeeFishWeb;
   using namespace BeeFishWebDB;
   using namespace BeeFishScript;
   using namespace BeeFishTest;

   using namespace std::filesystem;

   inline bool testAllFiles(string url, string directory);
   inline bool testFile(string url, string file, bool expect = true);
   inline bool testData(string url, string label, string data, bool expect = true);

   inline bool testVariables();
   inline bool testJSONPath(Database* db);

   inline bool test()
   {

      bool success = true;

      success = success &&
         testVariables();

      cout << "Test DB Server" << endl;

      remove(TEMP_FILENAME);

      DBServer* dbServer =
         new DBServer(
           WEB_SERVER_HOST, 8080, 1, TEMP_FILENAME
         );

      if (!dbServer->start())
         return false;

      success = success &&
         testJSONPath(dbServer);
/*
      if (success)
      {
         cout << "Testing 404 " << flush;

         stringstream stream;
         stream << "curl " << dbServer->url() << "bee"
            " -s | grep \"html\"";
         string command = stream.str();
         success &= (system(command.c_str()) == 0);
         outputSuccess(success);
      }

      if (success)
      {
         cout << "Testing post application/json " << flush;

         stringstream stream;
         stream << "curl " << dbServer->url() << "bee"
            " --header \"content-type: application/json\" " <<
            " --data 123 -s | grep \"true\"";
         string command = stream.str();
         cout << command << endl;
         success &= (system(command.c_str()) == 0);

         outputSuccess(success);
      }

      if (success)
      {
         cout << "Testing get " << flush;

         stringstream stream;
         stream << "curl -s " << dbServer->url() << "bee"
            " | grep 123";
         string command = stream.str();
         cerr << command << endl;
         success &= (system(command.c_str()) == 0);

         outputSuccess(success);

      }

      if (success) {
         success = testAllFiles(dbServer->url(), "tests");
      }
*/

      dbServer->stop();

      delete dbServer;

      std::filesystem::remove(TEMP_FILENAME);

      outputSuccess(success);

      return success;
   }

   inline bool testAllFiles(string url, string directory)
   {
      cout << "Testing all files in " << directory << endl;

      bool success = true;
      
      vector<string> files;

      for (const auto & entry : directory_iterator(directory))
      {
         files.push_back(entry.path());
      }

      sort(files.begin(), files.end());

      for (auto file : files) {
         if (success)
            success = testFile(url, file);
      }

      outputSuccess(success);

      return success;
   }
   
   inline bool testFile(string url, string file, bool expect)
   {
      cout << "Testing file " << file << " " << flush;
      stringstream stream;
      bool success = true;

      stream << 
         "curl -X POST " <<
         url  <<
         file << " "
         "-H \"Content-Type: application/json; charset=utf-8\" " <<
         "-H Expect: " <<
         "-T " << file << " -s | grep \"" << (expect ? "true" : "false") << "\" -q";

      string command = stream.str();
      success &= (system(command.c_str()) == 0);

      if (success && expect) {
         command = "curl -s " + url + file;
         success &= (system(command.c_str()) == 0);
      }

      outputSuccess(success);
      return success;
   }

   inline bool testData(string url, string label, string data, bool expect)
   {
      cout << "Testing data " << label << endl;
      stringstream stream;
      bool success = true;

      stream << 
         "curl " <<
         url << label <<
         " -H \"Content-Type: application/json; charset=utf-8\"" <<
         " -d " << data << " -s | grep \"" << (expect ? "true" : "false") << "\" -q";

      string command = stream.str();
      success &= (system(command.c_str()) == 0);
   
      if (success and expect) {
         string command2 = "curl " + url + label + " -s";

         cout << "   Outputing " << url + label << endl;
         success &= system(command2.c_str()) == 0;
      }

      outputSuccess(success);
      return success;
   }

   inline bool testVariables()
   {
      using namespace std;

      cout << "Testing variables" << endl;

      bool success = true;

      {
         Variable v = "Hello World";
         stringstream out;
         out << v;

         success = success &&
            testValue("\"Hello World\"", out.str());

      }

      {
         Variable v = "Hello\\World";
         stringstream out;
         out << v;

         success = success &&
            testValue("\"Hello\\\\World\"", out.str());
      }

      {
         Variable v = BeeFishScript::Object{{"🐝","🌎"}};
         stringstream out;
         out << v;

         success = success &&
            testValue("{\n   \"🐝\": \"🌎\"\n}", out.str());
      }
   
      BeeFishMisc::outputSuccess(success);

      return success;
   }
   
   inline bool testJSONPath(Database* db)
   {
      cout << "Testing JSON Path" << endl;

      bool success = true;
      BeeFishWeb::Path root(db);
      {
         cout << "\tTesting number " << flush;
         JSONPath path(root["json-number"]);
         Variable i = 22;
         path.setVariable(i);
         JSONPath path2(root["json-number"]);
         Variable i2 = path2.getVariable();
         success = ((int)i2 == 22);

         outputSuccess(success);
      }

      {
         cout << "\tTesting string " << flush;
         JSONPath path(root["json-string"]);
         Variable str = "My String";
         path.setVariable(str);

         JSONPath jsonPath(path);

         Variable var = jsonPath.getVariable();

         success = ((String)var == "My String");

         outputSuccess(success);
      }

      {
         cout << "\tTesting string retrieve " << flush;
         JSONPath path(root["json-string"]);
         Variable var = path.getVariable();

         success = ((String)var == "My String");

         outputSuccess(success);
      }

      {
         cout << "\tTesting array " << flush;
         JSONPath path(root["json-array"]);
         Variable array = Array{1,2,3};
         path.setVariable(array);
         JSONPath path2(root["json-array"]);
         Variable array2 = path2.getVariable();
         ArrayPointer a = array2;
         success = ((int)((*a)[2]) == 3);

         outputSuccess(success);
      }

      {
         cout << "\tTesting object " << flush;
         JSONPath path(root["json-object"]);
         Variable object =
            BeeFishScript::Object
            {
               {"a", 1},
               {"c", 2},
               {"e", BeeFishScript::Object()}
            };

         path.setVariable(object);

         JSONPath path2(root["json-object"]);
         Variable object2 = path2.getVariable();

         ObjectPointer o = object2;
         success = success &&
            ((Number)((*o)["c"]) == 2);

         Variable e = (*o)["e"];
         success = success &&
            e._type == BeeFishJSON::Type::OBJECT;

         outputSuccess(success);
      }

      {
         cout << "\tTesting complex " << flush;
         JSONPath path(root["json-complex"]);
         Variable object =
            BeeFishScript::Object
            {
               {"a", "b"},
               {"c", BeeFishScript::Array{1,2,3}},
               {"e", BeeFishScript::Object{
                     {"f", "g"}
                     }
               }
            };

         path.setVariable(object);
         JSONPath path2(root["json-complex"]);
         Variable object2 = path2.getVariable();
         ObjectPointer o = object2;
         ArrayPointer a = (*o)["c"];
         success = success &&
            ((Number)((*a)[1]) == 2);

         Variable f = (*o)["e"]["f"];
         success = success &&
            f._type == BeeFishJSON::Type::STRING &&
            (String)f == "g";
         outputSuccess(success);
      }

      {
         cout << "\tTesting object map " << flush;
         JSONPath path(root["json-map"]);
         Variable object =
            BeeFishScript::Object
            {
               {"a", "b"},
               {"c", BeeFishScript::Array{1,2,3}},
               {"e", BeeFishScript::Object{
                     {"f", "g"},
                     {"h", BeeFishScript::Array{1,2,3}}
                     }
               }
            };

         path.setVariable(object);
         Variable var =
            path["a"].getVariable();

         success = success &&
            (Number)(path["e"]["h"][0].getVariable()) == 1;

         outputSuccess(success);
      }

      outputSuccess(success);

      return success;
   }


}

#endif
