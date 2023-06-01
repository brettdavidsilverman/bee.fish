#ifndef BEE_FISH_WEB__TEST_REQUEST_H
#define BEE_FISH_WEB__TEST_REQUEST_H
#include "../Miscellaneous/Miscellaneous.hpp"

#include "../JSON/JSON.hpp"
#include "WebRequest.hpp"



namespace BeeFishWeb
{
   using namespace BeeFishJSON;
   using namespace BeeFishMisc;

   inline bool testHeaders();
   inline bool testWebRequestClass();

   inline bool testWebRequest()
   {
   
      bool success = true;

      if (success)
         success = testHeaders();

      if (success)
         success = testWebRequestClass();

      BeeFishMisc::outputSuccess(success);

      return success;
   }
   
   inline bool testHeaders()
   {
      cout << "Test Headers" << endl;
      
      bool success = true;
   
      string key, value;


      success &=
         testPattern(
            _Header(key, value),
            "Header: value\r\n",
            true
         );

      success &=
         testValue(
            "Header",
            key
         );

      success &=
         testValue(
            "value",
            value
         );

      Header header;

      success &=
         testPattern(
            header,
            "Key: value\r\n",
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

      //And headers = Headers() and newLine;
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
      
      cout << "Test WebRequest" << endl;
      
      bool success = true;
      
      string method, url, version;

      success &=
         testPattern(
            _WebRequest(method, url, version),
            "GET / HTTP/1.1\r\n" \
            "Host: " HOST "\r\n" \
            "\r\n",
            true
         );

      success &=
         testValue("GET", method);

      success &=
         testValue("/", url);

       success &=
         testValue("HTTP/1.1", version);


      WebRequest webRequest;

      success &=
         testPattern(
            webRequest,
            "GET / HTTP/1.1\r\n" \
            "Host: " HOST "\r\n" \
            "\r\n",
            true
         );

      success &=
         testValue("GET", webRequest._method);

      success &=
         testValue("/", webRequest._url);

       success &=
         testValue("HTTP/1.1", webRequest._version);

      BeeFishMisc::outputSuccess(success);

      return success;
      
   }

}

#endif
