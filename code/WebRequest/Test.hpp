#ifndef BEE_FISH_WEB__TEST_REQUEST_H
#define BEE_FISH_WEB__TEST_REQUEST_H
#include "../Miscellaneous/Miscellaneous.hpp"

#include "../JSON/JSON.hpp"
#include "../Test/Test.hpp"

#include "WebRequest.hpp"
#include "../WebServer/WebServer.hpp"


namespace BeeFishTest
{
   using namespace BeeFishWeb;
   using namespace BeeFishParser;
   using namespace BeeFishJSON;
   using namespace BeeFishMisc;
   using namespace BeeFishTest;

   Database* database = nullptr;
   Path* root = nullptr;
   WebServer* webServer = nullptr;

   inline bool testHeaders();
   inline bool testWebRequestClass();

   inline bool testWebRequest()
   {
   
      bool success = true;

      database = new Database();
      root = new Path(database);
      webServer = new WebServer(
         "localhost", 8000, 2
      );

      if (success)
         success = testHeaders();

      if (success)
         success = testWebRequestClass();

      BeeFishMisc::outputSuccess(success);

      std::string tempfile = database->filename();
      remove(tempfile.c_str());
      
      delete root;
      delete database;
      delete webServer;

      return success;
   }
   
   inline bool testHeaders()
   {
      cout << "Test Headers" << endl;
      
      bool success = true;

      Header header;

      success &=
         testPattern(
            header,
            "key: value\r\n",
            true
         );

      success &=
         testValue(
            "key",
            header._key
         );

      success &=
         testValue(
            "value",
            header._value
         );


      Headers headers;

      success &=
         testPattern(
            headers,
            "Header1: value1\r\n" \
            "Header2: value2\r\n" \
            "\r\n",
            true
         );

      success &=
         testValue(
            "value1",
            headers["header1"]
         );

      success &=
         testValue(
            "value2",
            headers["header2"]
         );

      outputSuccess(success);

      return success;
   }

   inline bool testWebRequestClass()
   {
      
      cout << "Test WebRequest Class" << endl;
      
      bool success = true;
      
      string host = WEB_SERVER_HOST;

      WebRequest webRequest(webServer);

      success &=
         testPattern(
            webRequest,
            "GET / HTTP/1.1\r\n" \
            "Host: " + host + "\r\n" \
            "\r\n",
            true
         );

      success &=
         testValue("GET", webRequest._method);

      success &=
         testValue("/", webRequest._url);

      success &=
         testValue("HTTP/1.1", webRequest._version);

      success &=
         testValue(host, webRequest._headers["host"]);

      WebRequest webRequestJSON(webServer);

      success &=
         testPattern(
            webRequestJSON,
            "POST / HTTP/1.1\r\n" \
            "Host: " + host + "\r\n" \
            "Content-type: application/json\r\n" \
            "\r\n" \
            "{}",
            true
         );

      BeeFishMisc::outputSuccess(success);

      return success;
      
   }


}

#endif
