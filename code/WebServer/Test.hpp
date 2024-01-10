#ifndef BEE_FISH__WEB_SERVER__TEST__HPP
#define BEE_FISH__WEB_SERVER__TEST__HPP

#include <thread>
#include <stdlib.h>

#include "../Database/Database.hpp"
#include "WebServer.hpp"

namespace BeeFishTest {

   using namespace std;   
   using namespace BeeFishMisc;
   using namespace BeeFishWeb;
   
   inline bool testStartStop();

   inline bool testWebServer() {
      using namespace std;

      bool success = true;


      if (success)
         success &= testStartStop();

      if (success)
         cout << "WebServer tests pass";
      else
         cout << "WebServer tests failed";

      outputSuccess(success);

      return success;
   }

   inline bool testStartStop() {

      using namespace std;

      cout << "WebServer start/stop on port " << TEST_SERVER_PORT << endl;

      bool success = true;
      
      cout << endl << "\t" << "Starting" << endl;

      WebServer testServer("localhost", TEST_SERVER_PORT);
      
      outputSuccess(true);

      if (success) {
         cout << endl << "\t" << "Stopping" << endl;

         testServer.stop();
      }      

      outputSuccess(success);

    
      return success;
      
   }

   
    
}

#endif