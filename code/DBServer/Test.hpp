#ifndef BEE_FISH__TEST_HPP
#define BEE_FISH__TEST_HPP
#include <filesystem>
#include <bits/stdc++.h>
#include "../Miscellaneous/Miscellaneous.hpp"
#include "DBServer.hpp"

#define TEMP_FILENAME "/var/tmp/bee.fish.tmp"

namespace BeeFish
{
   using namespace BeeFishJSON;
   using namespace BeeFishMisc;
   using namespace BeeFishWeb;
   using namespace BeeFishWebDB;
   using namespace std::filesystem;

   inline bool testAllFiles(string url, string directory);
   inline bool testFile(string url, string file, bool expect = true);
   inline bool testData(string url, string label, string data, bool expect = true);
   inline bool test()
   {

      bool success = true;

      cout << "Test DB Server" << endl;

      remove(TEMP_FILENAME);

      DBServer* dbServer =
         new DBServer(
           WEB_SERVER_HOST, 8080, 1, TEMP_FILENAME
         );

      if (!dbServer->start())
         return false;

    //  success = testFile(dbServer->url(), "tests/04-Object.json");

    //  outputSuccess(success);
// *******
    //  success = false;

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

}

#endif
