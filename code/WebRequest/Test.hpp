#ifndef BEE_FISH_WEB__TEST_REQUEST_H
#define BEE_FISH_WEB__TEST_REQUEST_H
#include "../Miscellaneous/Miscellaneous.hpp"

#include "../JSON/JSON.hpp"
#include "../Test/Test.hpp"

#include "WebRequest.hpp"



namespace BeeFishWeb
{
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

      map<string, string> _headers;

      Headers headers(
         [&_headers](Header* header) {
            _headers.emplace(
               header->_key,
               header->_value
            );
         }
      );
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
            _headers["header1"]
         );

      success &=
         testValue(
            "value2",
            _headers["header2"]
         );

      outputSuccess(success);

      return success;
   }

   inline bool testWebRequestClass()
   {
      
      cout << "Test WebRequest" << endl;
      
      bool success = true;
      
      string method, url, version;
      map<string, string> headers;

      string host = WEB_SERVER_HOST;

      success &=
         testPattern(
            _WebRequest(method, url, version, headers),
            "GET / HTTP/1.1\r\n" \
            "Host: " + host + "\r\n" \
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
      BeeFishMisc::outputSuccess(success);

      return success;
      
   }

   inline bool testURL()
   {
      
      cout << "Test URL" << endl;
      
      std::string path;
      std::string fullPath;

      auto onpath =
      [&path, &fullPath](Parser* parser)
      {
         cout << "{" << path << "}";
         fullPath += "/" + path;
         path = "";
      };

      bool success = true;

      fullPath = "";
      success &= testPattern(
         URL(path, onpath),
         "/",
         nullopt
      );

      success &= testValue(
         "",
         fullPath
      );
      
      fullPath = "";

      success &= testPattern(
         URL(path, onpath),
         "/path/",
         nullopt
      );

      success &= testValue(
         "/path",
         fullPath
      );
      
      fullPath = "";

      success &= testPattern(
         URL(path, onpath),
         "/path1/path2/",
         nullopt
      );

      success &= testValue(
         "/path1/path2",
         fullPath
      );

      fullPath = "";

      success &= testPattern(
         URL(path, onpath) and newLine,
         "/path3/path4\r\n",
         true
      );

      success &= testValue(
         "/path3/path4",
         fullPath
      );
      
      BeeFishMisc::outputSuccess(success);

      return success;
   }
    

}

#endif
