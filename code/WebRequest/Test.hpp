#ifndef BEE_FISH_WEB__TEST_REQUEST_H
#define BEE_FISH_WEB__TEST_REQUEST_H
#include "../Miscellaneous/Miscellaneous.hpp"

#include "../JSON/JSON.hpp"
#include "../Test/Test.hpp"

#include "WebRequest.hpp"



namespace BeeFishWeb
{
   using namespace BeeFishParser;
   using namespace BeeFishJSON;
   using namespace BeeFishMisc;
   using namespace BeeFishTest;

   inline bool testHeaders();
   inline bool testWebRequestClass();
   inline bool testURL();

   inline bool testWebRequest()
   {
   
      bool success = true;

      if (success)
         success = testHeaders();

      if (success)
         success = testWebRequestClass();

      if (success)
         success = testURL();

      BeeFishMisc::outputSuccess(success);

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

      WebRequest webRequest;

      success &=
         testPattern(
            webRequest,
            "GET / HTTP/1.1\r\n" \
            "Host: " + host + "\r\n" \
            "\r\n",
            nullopt
         );

      success &=
         testValue("GET", webRequest._method);

      success &=
         testValue("/", webRequest._url);

      success &=
         testValue("HTTP/1.1", webRequest._version);

      success &=
         testValue(host, webRequest._headers["host"]);

      WebRequest webRequestJSON;
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

   inline bool testURL()
   {
      
      cout << "Test URL" << endl;
      
      std::string fullPath;

      auto onpath =
      [&fullPath](std::string path)
      {
        // cout << "{" << path << "}";
         fullPath += "/" + path;
         return true;
      };

      bool success = true;

      fullPath = "";
      success &= testPattern(
         URL(onpath),
         "/",
         nullopt
      );

      success &= testValue(
         "",
         fullPath
      );
      
      fullPath = "";

      success &= testPattern(
         URL(onpath),
         "/path/",
         nullopt
      );

      success &= testValue(
         "/path",
         fullPath
      );
      
      fullPath = "";

      success &= testPattern(
         URL(onpath),
         "/path1/path2/",
         nullopt
      );

      success &= testValue(
         "/path1/path2",
         fullPath
      );

      fullPath = "";

      success &= testPattern(
         URL(onpath),
         "/path3/path4?",
         true
      );

      success &= testValue(
         "/path3/path4",
         fullPath
      );

      success &= testPattern(
         URL(onpath),
         "/path5/path6/",
         nullopt
      );

      success &= testPattern(
         URL(onpath),
         "/path9/path10/?",
         true
      );

      BeeFishMisc::outputSuccess(success);

      return success;
   }

    

}

#endif
