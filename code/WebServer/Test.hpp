#ifndef BEE_FISH__WEB_SERVER__TEST__HPP
#define BEE_FISH__WEB_SERVER__TEST__HPP

#include <thread>
#include <stdlib.h>

#include "WebServer.hpp"

namespace BeeFishWeb {
   
   bool testStartStop();

   inline bool testWebServer() {
      using namespace std;

      bool success = true;

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

      cout << "WebServer start/stop on port 8080" << endl;

      bool success = true;

      WebServer testServer(8080, 2);
      
      testServer.start();
      
      stringstream stream;
      stream << "curl " << testServer.host();
      string command = stream.str();
      success &= (system(command.c_str()) == 0);

      testServer.stop();

      outputSuccess(success);

    
      return success;
      
   }

}

#endif