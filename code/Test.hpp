#ifndef BEE_FISH__TEST_HPP
#define BEE_FISH__TEST_HPP
#include <filesystem>
#include "Miscellaneous/Miscellaneous.hpp"
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
         WEB_SERVER_HOST, 8080, 2, TEMP_FILENAME
      );

      dbServer.start();

      cout << "Testing 404" << endl;
      {
         stringstream stream;
         stream << "curl " << dbServer.url() << "bee"
            "| grep \"404\"";
         string command = stream.str();
         success &= (system(command.c_str()) == 0);
      }

      cout << "Testing post application/json" << endl;
      {
         stringstream stream;
         stream << "curl " << dbServer.url() << "bee"
            " --header \"content-type: application/json\" " <<
            " --data 123 | grep 123";
         string command = stream.str();
         success &= (system(command.c_str()) == 0);
      }

      cout << "Testing get" << endl;
      {
         stringstream stream;
         stream << "curl " << dbServer.url() << "bee"
            " | grep 123";
         string command = stream.str();
         success &= (system(command.c_str()) == 0);
      }

      dbServer.stop();

      std::filesystem::remove(TEMP_FILENAME);

      outputSuccess(success);

      return success;
   }
   

}

#endif
