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

   inline bool test()
   {

      bool success = true;

      cout << "Test DB Server" << endl;

      std::filesystem::remove(TEMP_FILENAME);

      DBServer dbServer(
         WEB_SERVER_HOST, 8080, 1, TEMP_FILENAME
      );

      dbServer.start();

      if (success)
      {
         cout << "Testing 404 ";

         stringstream stream;
         stream << "curl " << dbServer.url() << "bee"
            " -s | grep \"html\"";
         string command = stream.str();
         success &= (system(command.c_str()) == 0);
         outputSuccess(success);
      }

      if (success)
      {
         cout << "Testing post application/json ";

         stringstream stream;
         stream << "curl " << dbServer.url() << "bee"
            " --header \"content-type: application/json\" " <<
            " --data 123 -s | grep success";
         string command = stream.str();
         success &= (system(command.c_str()) == 0);

         outputSuccess(success);
      }

      if (success)
      {
         cout << "Testing get ";

         stringstream stream;
         stream << "curl " << dbServer.url() << "bee"
            " -s | grep 123";
         string command = stream.str();
         success &= (system(command.c_str()) == 0);

         outputSuccess(success);

      }

      dbServer.stop();

      std::filesystem::remove(TEMP_FILENAME);

      outputSuccess(success);

      return success;
   }
   

}

#endif
