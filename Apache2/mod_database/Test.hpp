#ifndef BEE_FISH__APACHE2__TEST_HPP
#define BEE_FISH__APACHE2__TEST_HPP
#include <filesystem>
#include <bits/stdc++.h>
#include "Miscellaneous/Miscellaneous.hpp"
#include "Test/Test.hpp"

namespace BeeFishApache2 {

   using namespace BeeFishJSON;
   using namespace BeeFishMisc;
   using namespace BeeFishDatabase;
   using namespace BeeFishScript;
   using namespace BeeFishTest;

   using namespace std::filesystem;

   inline bool testAllFiles(string url, string directory);
   inline bool testFile(string url, string file, bool expect = true);
   
   inline bool test() {
      cout << "Testing Apache2" << endl;
      bool success = true;
      
      string url = "https://bee.fish/";
     
      success = success &&
         testAllFiles(url, "tests");
         
      outputSuccess(success);
      
      return success;
   }

   std::optional<bool> testParser(Parser* parser) {

      cin >> *parser;
 
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
         files.push_back(entry.path());
      }

      sort(files.begin(), files.end());

      for (auto file : files) {
         if (success)
            success = testFile(url, file);
         else
            break;
      }

      outputSuccess(success);

      return success;
   }
   
   inline bool testFile(string url, string file, bool expect)
   {
      cout << "\tTesting "
           << url << file
           << endl;

      stringstream stream;
      bool success = true;

      stream << 
         "curl -X POST " <<
         url  <<
         file << " "
         "-H \"Content-Type: application/json; charset=utf-8\" " <<
         "-H Expect: " <<
         "-T " << file
         << " -s > "
         << TEMP_DIRECTORY
          << "test.curl.txt";

      string command = stream.str();

      success &= (system(command.c_str()) == 0);

      if (success && expect) {
          
         // Compare the files
         stringstream stream;
         stream  << "diff -s -Z "
                 << TEMP_DIRECTORY
                 << "test.curl.txt"
                 << " "
                 << file;
                
         command = stream.str();
         
         success = success && 
            (system(command.c_str()) == 0);
      }

      outputSuccess(success);
      return success;
   }
   
}

#endif