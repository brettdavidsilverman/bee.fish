#ifndef BEE_FISH__APACHE2__TEST_HPP
#define BEE_FISH__APACHE2__TEST_HPP
#include <filesystem>
#include <bits/stdc++.h>
#include "Miscellaneous/Miscellaneous.hpp"

namespace BeeFishApache2 {

   using namespace BeeFishJSON;
   using namespace BeeFishMisc;
   using namespace BeeFishDatabase;
   using namespace BeeFishScript;
   using namespace BeeFishTest;

   using namespace std::filesystem;

   inline bool testAllFiles(string url, string directory);
   inline bool testFile(string url, string directory, string file, bool expect = true);
   
   inline bool test() {
      cout << "Testing Apache2" << endl;
      bool success = true;
      
      string url = HOST;
     
      success = success &&
         testAllFiles(url, TEST_DIRECTORY);
         
      outputSuccess(success);
      
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