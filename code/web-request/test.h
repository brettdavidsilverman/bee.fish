#ifndef BEE_FISH_WEB__TEST_REQUEST_H
#define BEE_FISH_WEB__TEST_REQUEST_H

#include "../test/test.h"

#include "../json/json-parser.h"
#include "web-request.h"
#include "../Script/Script.hpp"

#define SERVER

#ifdef SERVER
using namespace std::filesystem;
#endif

using namespace BeeFishTest;

namespace BeeFishWeb
{

   inline bool testURL();
#ifdef SERVER
   inline bool testWebRequest();
   inline bool testParts();
   inline bool testStreams();
   inline bool testJSON();
#endif

   inline bool testRequest()
   {
   
      bool ok = true;

      //ok = ok && testJSON();
      //return ok;

      ok = ok && testURL();

#ifdef SERVER
      ok = ok && testWebRequest();
      ok = ok && testParts();
      ok = ok && testStreams();
      ok = ok && testJSON();
#endif

      if (ok)
         cout << "SUCCESS" << endl;
      else
         cout << "FAIL" << endl;
         
      return ok;
   }
   
   inline bool testURL() {
      cout << "Test URL" << endl;

      bool ok = true;

      WebRequest::URL::Hex hex;
      Parser hexParser(hex);
      hexParser.read("a");
      ok = ok && testResult("URL Hex is 'a'", 
         hexParser.result() == true && 
         hex.value() == Char('a'));
      
      WebRequest::URL::HexCharacter hexCharacter;
      Parser hexCharacterParser(hexCharacter);
      hexCharacterParser.read("%38");
      hexCharacterParser.eof();
      
      ok = ok && testResult("URL hex Parser", 
         hexCharacterParser.result() == true);
       
      ok = ok && testResult("URL hex character", 
         hexCharacter.result() == true);
            
      ok = ok && testResult("URL hex value is '8'", 
         hexCharacter.character() == Char('8'));      assert(ok);
      
      WebRequest::URL::Path path;
      Parser pathParser(path);
      pathParser.read("Hello%20World%25");
      pathParser.eof();

      ok = ok && testResult("Path with escaped space is \"Hello World%\"",
         path.result() == true &&
         path.value() == "Hello World%"
      );
      
      ok = ok && testResult("Path with escaped space is \"Hello World%\"",
         path.result() == true &&
         path.value() == "Hello World%"
      );

      WebRequest::URL url;
      Parser urlWithQueryParser(url);
      urlWithQueryParser.read("/beehive/settings/?key1=value1&key2=value2&key3 HTTP/1.1");
      urlWithQueryParser.eof();
   
      ok = ok && testResult(
         "Path with query",
         url.query().result() == true
      );

      ok = ok && testResult(
         "Path with query value",
         url.query().value() == "key1=value1&key2=value2&key3"
      );

      ok = ok && testResult(
         "Path with query key 1",
         url.query()["key1"] == "value1"
      );

      ok = ok && testResult(
         "Path with query key 2",
         url.query()["key2"] == "value2"
      );


      ok = ok && testResult(
         "Path with query key 3",
         url.query().contains("key3") && 
         url.query()["key3"].length() == 0
      );

      WebRequest::URL url2;
      Parser urlWithQueryParser2(url2);
      urlWithQueryParser2.read("/beehive/settings/?key=hello%20world HTTP/1.1");
      urlWithQueryParser2.eof();
      
      ok = ok && testResult(
         "Path with escaped query",
         url2.query()["key"] == "hello world"
      );

      return ok;


   }

#ifdef SERVER
   inline bool testWebRequest()
   {
      using namespace BeeFishJSON;

      cout << "Test request" << endl;
      
      bool ok = true;
      
      BeeFishWeb::WebRequest requestHeadersOnly;
      ok = ok && testFile(
         "WebRequest with only headers",
         WWW_ROOT_DIRECTORY "/code/web-request/tests/request.txt",
         requestHeadersOnly,
         true
      );
      
      ok = ok && testResult(
         "WebRequest get",
         requestHeadersOnly.method() == "GET"
      );
      
      ok = ok && testResult(
         "WebRequest has no json",
         requestHeadersOnly.hasJSON() == false
      );
      
      BeeFishWeb::WebRequest requestBody;
      ok = ok && testFile(
         "WebRequest with body",
         WWW_ROOT_DIRECTORY "/code/web-request/tests/request-body.txt",
         requestBody,
         true
      );
      
      ok = ok && testResult(
         "WebRequest has json",
         requestBody.hasJSON() == true
      );
      
      BString name;
      bool hit = false;

      BeeFishJSON::JSONParser::OnValue invokeOnName = 
         [&name, &hit](const BString& key, JSON& json) 
         {
            name = json.value();
            hit = true;
         };

      BeeFishWeb::WebRequest requestFull;
      JSONParser parser(requestFull);
      parser.invokeValue("name", invokeOnName);

      ok = ok && testFile(
         parser,
         "WebRequest with full json",
         WWW_ROOT_DIRECTORY "/code/web-request/tests/request-full.txt",
         requestFull,
         true
      );
      
      ok = ok && testResult(
         "WebRequest full has json",
         requestFull.hasJSON() == true
      );

      ok = ok && testResult(
         "WebRequest full has name",
         hit == true
      );
      
      ok = ok && testResult(
         "WebRequest full name is Brett",
         name == "Brett"
      );
      
      
      BeeFishMisc::optional<BString> name2;

      BeeFishWeb::WebRequest request2;
      JSONParser parser2(request2);
      parser2.captureValue("name", name2);

      ok = ok && testFile(
         parser2,
         "WebRequest with full json 2",
         WWW_ROOT_DIRECTORY "/code/web-request/tests/request-full.txt",
         request2,
         true
      );
      
      ok = ok && testResult(
         "WebRequest full has name 2",
         name2.has_value()
      );
      
      ok = ok && testResult(
         "WebRequest full name is Brett 2",
         name2 == BString("Brett")
      );

      BeeFishWeb::WebRequest urlWebRequest;
      ok = ok && testFile(
         "WebRequest with path and query",
         WWW_ROOT_DIRECTORY "/code/web-request/tests/path.txt",
         urlWebRequest,
         true
      );
      
      ok = ok && testResult(
         "WebRequest path is /path",
         urlWebRequest.path() == "/path"
      );
      
      ok = ok && testResult(
         "WebRequest query is query",
         urlWebRequest.query() == "query"
      );
      
      BeeFishWeb::WebRequest escapedUrlWebRequest;
      ok = ok && testFile(
         "WebRequest with escaped path and query",
         WWW_ROOT_DIRECTORY "/code/web-request/tests/escaped-path.txt",
         escapedUrlWebRequest,
         true
      );
      
      ok = ok && testResult(
         "WebRequest escaped path is /path",
         escapedUrlWebRequest.path() == "/path"
      );
      
      ok = ok && testResult(
         "WebRequest escaped query is query<space>query",
         escapedUrlWebRequest.query() == "query%20query"
      );

      BeeFishWeb::WebRequest postWebRequest;
      
      ok = ok && testFile(
         "Post with encoded name value pairs",
         WWW_ROOT_DIRECTORY "/code/web-request/tests/post.txt",
         postWebRequest,
         true
      );

      BeeFishWeb::WebRequest postWebRequest2;
      BString body;
      postWebRequest2.setOnData(
         [&body](const std::string& data) {
            body = data;
         }
      );

      // Post with anything
      ok = ok && testFile(
         "Post with content length text",
         WWW_ROOT_DIRECTORY "/code/web-request/tests/request-text.txt",
         postWebRequest2,
         true
      );

      postWebRequest2.flush();
      

      ok = ok && testResult(
         "Post with hello world body",
         body == "{\"hi\":\"world\"}"
      );

      // Post with anything
      BeeFishWeb::WebRequest postWebRequest3;
      ok = ok && testFile(
         "Post with zero content length",
         WWW_ROOT_DIRECTORY "/code/web-request/tests/zero-content-length.txt",
         postWebRequest3,
         true
      );

      postWebRequest3.flush();


      return ok;
   }
   
   inline bool testParts()
   {
      
      cout << "Test request parts" << endl;
      
      bool ok = true;
      
      
      BeeFishWeb::WebRequest request;
      
      ok = ok && testFile(
         "WebRequest part 0",
         WWW_ROOT_DIRECTORY "/code/web-request/tests/request-part-0.txt",
         request,
         BeeFishMisc::nullopt
      );
      
      ok = ok && testFile(
         "WebRequest part 1",
         WWW_ROOT_DIRECTORY "/code/web-request/tests/request-part-1.txt",
         request,
         BeeFishMisc::nullopt
      );
      
      ok = ok && testFile(
         "WebRequest part 2",
         WWW_ROOT_DIRECTORY "/code/web-request/tests/request-part-2.txt",
         request,
         true
      );
      
      ok = ok && testResult(
         "WebRequest object is valid",
         request.hasJSON()
      );

      cout << endl;
      
      return ok;
      
   }


   inline bool testStreams()
   {
      
      cout << "Test Streams" << endl;

      using namespace BeeFishJSON;
      
      bool ok = true;
        
      BeeFishBString::BStream::OnBuffer onimage =
         [](const std::string& buffer) {
         };

      BeeFishWeb::WebRequest request;
      JSONParser parser(request);

      parser.streamValue("image", onimage);

      ok = ok && testFile(
         parser,
         "WebRequest Image JSON",
         WWW_ROOT_DIRECTORY "/code/web-request/tests/image-json.txt",
         request,
         true
      );

      cout << endl;

      return ok;
   }

   inline bool testJSON()
   {
      
      cout << "Test JSON" << endl;

      using namespace BeeFishJSON;
      
      bool ok = true;
        
      BeeFishWeb::WebRequest request;
      JSONParser parser(request);
      //BeeFishBScript::BScriptParser parser(request);

      ok = ok && testFile(
         parser,
         "POST with hello world sample",
         WWW_ROOT_DIRECTORY "/code/web-request/tests/post-json.txt",
         request,
         true
      );

      
      BeeFishWeb::WebRequest request2;
      JSONParser parser2(request2);
      //BeeFishBScript::BScriptParser parser2(request2);
      //BeeFishBScript::BScriptParser parser(request);

      ok = ok && testFile(
         parser2,
         "POST to settings",
         WWW_ROOT_DIRECTORY "/code/web-request/tests/post-settings.txt",
         request2,
         true
      );

      cout << endl;


      return ok;
   }
#endif


}

#endif
