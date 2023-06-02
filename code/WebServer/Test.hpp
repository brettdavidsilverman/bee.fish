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
         cout << "WebServer tests pass" << endl;
      else
         cout << "WebServer tests failed" << endl;

      return success;
   }

   inline bool testStartStop() {

      using namespace std;

      cout << "WebServer start/stop on port 8080" << endl;

      bool success = true;

      WebServer testServer(8080, 1);
      
      testServer.start();
      //BeeFishMisc::sleep(2);

      //string command = "curl " + testServer.host();
         
      std::string command = "curl " + testServer.host() + " > /dev/null 2>&1";

      for (int i = 0; i < 1000; ++i) {
         if (system(command.c_str()) != 0) {
            cout << "WebServer tests fail" << endl;
            success = false;
            break;
         }
         if (i % 100 == 0)
            cerr << i << endl;
      }
       

      
      testServer.stop();


      if (success)
         cout << "Ok" << endl;
      else
         cout << "Fail" << endl;

    
      return success;
      
   }

}

#endif