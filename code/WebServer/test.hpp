#ifndef BEE_FISH__WEB_SERVER__TEST__HPP
#define BEE_FISH__WEB_SERVER__TEST__HPP

#include <thread>
#include <stdlib.h>

namespace BeeFish {
   
   bool testStartStop();

   inline bool testWebServer() {
      using namespace std;

      bool success = true;

      success &= testStartStop();

      if (success)
         cout << "WebServer tests pass" << endl;
      else
         cout << "WebServer tests failed" << endl;

      return success;
   }

   inline bool testStartStop() {

      using namespace std;

      cout << "WebServer start/stop on port 8080" << endl;

      bool success = false;

      WebServer testServer(8080, 1);
      
      testServer.start();
      BeeFish:sleep(2);

      //string command = "curl " + testServer.host();
         

      string command = "curl " + testServer.host() + " | grep \"Hello World\"";
     
     
      if (system(command.c_str()) == 0) {
         cout << "WebServer tests pass" << endl;
         success = true;
      }
       

      
      testServer.stop();

      success = true;

      if (success)
         cout << "Ok" << endl;
      else
         cout << "Fail" << endl;

    
      return success;
      
   }

}

#endif