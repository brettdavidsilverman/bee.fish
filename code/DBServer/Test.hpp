#ifndef BEE_FISH__TEST_HPP
#define BEE_FISH__TEST_HPP
#include <filesystem>
#include "../Miscellaneous/Miscellaneous.hpp"
#include "DBServer.hpp"

#define TEMP_FILENAME "/var/tmp/temp.data"

namespace BeeFish
{
   using namespace BeeFishJSON;
   using namespace BeeFishMisc;
   using namespace BeeFishWeb;
   using namespace BeeFishWebDB;

   inline bool testFile(string url, string file);

   inline bool test()
   {

      bool success = true;

      cout << "Test DB Server" << endl;

      std::filesystem::remove(TEMP_FILENAME);

      DBServer dbServer(
         WEB_SERVER_HOST, 8080, 1, TEMP_FILENAME
      );

      if (!dbServer.start())
         return false;

      if (success)
      {
         cout << "Testing 404 " << flush;

         stringstream stream;
         stream << "curl " << dbServer.url() << "bee"
            " -s | grep \"html\"";
         string command = stream.str();
         success &= (system(command.c_str()) == 0);
         outputSuccess(success);
      }

      if (success)
      {
         cout << "Testing post application/json " << flush;

         stringstream stream;
         stream << "curl " << dbServer.url() << "bee"
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
         stream << "curl " << dbServer.url() << "bee"
            " -s | grep 123";
         string command = stream.str();
         success &= (system(command.c_str()) == 0);

         outputSuccess(success);

      }

      if (success)
      {
         cout << "Testing test files " << flush;

         success = success &&
            testFile(dbServer.url(), "Simple.json");

         success = success &&
            testFile(dbServer.url(), "TestError1.json");

         success = success &&
            testFile(dbServer.url(), "LargeNumber.json");

         success = success &&
            testFile(dbServer.url(), "large.json");

         outputSuccess(success);

      }

      dbServer.stop();

      std::filesystem::remove(TEMP_FILENAME);

      outputSuccess(success);

      return success;
   }
   
   inline bool testFile(string url, string file)
   {
      cout << "Testing file " << file << endl;
      stringstream stream;
      bool success = true;

      stream << 
         "curl -X POST " <<
         url <<
         file << " "
         "-H \"Content-Type: application/json; charset=utf-8\" " <<
         "-H Expect: " <<
         "-T tests/" << file << " -s | grep \"true\"";

      string command = stream.str();
      cerr << "   Executing..." << endl << command << endl;
      success &= (system(command.c_str()) == 0);

      outputSuccess(success);
      return success;
   }

}

#endif
