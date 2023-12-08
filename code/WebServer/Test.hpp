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

   WebServer* _testWebServer = nullptr;

   inline bool testStartStop();
   inline bool testURLs();
   inline bool testURL(string);
   
   inline bool testWebServer() {
      using namespace std;

      bool success = true;

      success &= testStartStop();


      if(success) {
         _testWebServer = new WebServer("localhost", 8000, 2);
         success = _testWebServer->start();
      }


      if (success)
         success &= testURLs();

      if (success)
         cout << "WebServer tests pass";
      else
         cout << "WebServer tests failed";

      if (_testWebServer)
         delete _testWebServer;

      outputSuccess(success);

      return success;
   }

   inline bool testStartStop() {

      using namespace std;

      cout << "WebServer start/stop on port 8080" << endl;

      bool success = true;

      WebServer testServer("localhost", 8000);
      
      cout << endl << "\t" << "Starting" << endl;

      if (!testServer.start()) {
         outputSuccess(false);
         return false;
      }

      outputSuccess(true);

      if (success) {
         cout << endl << "\t" << "Stopping" << endl;

         testServer.stop();
      }      

      outputSuccess(success);

    
      return success;
      
   }

   inline bool testURL(string url) {

      bool success = true;
      
      string fullUrl;
      string fullPath;

      _testWebServer->_onsegment =
         [&fullPath](std::string segment)
         {
            cout << "{" << segment << "}" << endl;

            fullPath = fullPath + "/" + segment;
            return true;
         };

      fullUrl = _testWebServer->url();
      fullUrl += url;
      
      cerr << "\t" << fullUrl << ":" << flush;

      stringstream stream;

      stream << 
         "curl " <<
         fullUrl  << " ";

      string command = stream.str();
      
      success &= (system(command.c_str()) == 0);

      if (success) {
         success &= testValue(url, fullPath);
      }
      BeeFishMisc::outputSuccess(success);
   
      return success;
   }

   inline bool testURLs()
   {
      
      cout << "Test URLs" << endl;


      bool success = true;

      success &= testURL("");

      success &= testURL("path/");

      success &= testURL(
         "path1/path2/"
      );

      success &= testURL(
         "path3/path4?"
      );

      success &= testURL(
         "path5/path6/"
      );

      success &= testURL(
         "path9/path10 "
      );
      
      success &= testURL(
         "path11/path12?path13.path14 "
      );

      BeeFishMisc::outputSuccess(success);

      return success;
   }

    
}

#endif