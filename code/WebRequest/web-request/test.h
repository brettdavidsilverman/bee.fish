#ifndef BEE_FISH_WEB__TEST_REQUEST_H
#define BEE_FISH_WEB__TEST_REQUEST_H
#include "../test/test.h"

#include "../json/json-parser.h"
#include "web-request.h"
#include "../b-script/b-script-parser.h"

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

      //ok &= testJSON();
      //return ok;

      ok &= testURL();

#ifdef SERVER
      ok &= testWebRequest();
      ok &= testParts();
      ok &= testStreams();
      ok &= testJSON();
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
      ok &= testResult("URL Hex is 'a'", 
         hexParser.result() == true && 
         hex.value() == Char('a'));
      
      WebRequest::URL::HexCharacter hexCharacter;
      Parser hexCharacterParser(hexCharacter);
      hexCharacterParser.read("%38");
      
      ok &= testResult("URL hex value is '8'", 
         hexCharacter.result() == true && 
         hexCharacter.character() == Char('8'));
            
      WebRequest::URL::Path path;
      Parser pathParser(path);
      pathParser.read("Hello%20World%25");
      ok &= testResult("Path with escaped space is \"Hello World%\"",
         path.result() == BeeFishMisc::nullopt &&
         path.value() == "Hello World%"
      );

      cout << "Path: " << path.result() << ": " << path.value() << endl;

      WebRequest::URL url;
      Parser urlWithQueryParser(url);
      urlWithQueryParser.read("/beehive/settings/?key1=value1&key2=value2&key3 HTTP/1.1");

      cout << "Path With Query: " << url.query().result() << "\"" << url.query().value() << "\"" << endl;

   
      ok &- testResult(
         "Path with query",
         url.query().result() == true
      );

      ok &- testResult(
         "Path with query value",
         url.query().value() == "key1=value1&key2=value2&key3"
      );

      ok &- testResult(
         "Path with query key 1",
         url.query()["key1"] == "value1"
      );

      ok &- testResult(
         "Path with query key 2",
         url.query()["key2"] == "value2"
      );


      ok &- testResult(
         "Path with query key 3",
         url.query().contains("key3") && 
         url.query()["key3"].length() == 0
      );

      WebRequest::URL url2;
      Parser urlWithQueryParser2(url2);
      urlWithQueryParser2.read("/beehive/settings/?key=hello%20world HTTP/1.1");

      ok &- testResult(
         "Path with escaped query",
         url.query()["key"] == "hello world"
      );

/*
      WebRequest::URL::Path hexCharacterSequence;
      Parser sequenceParser(hexCharacterSequence);
      sequenceParser.read("%F0%9F%90%9D");

      cerr << "\"" << hexCharacterSequence.value() << "\"" << endl;

      ok &= testResult("URL hex character sequence is '🐝'", 
         hexCharacterSequence.result() == BeeFishMisc::nullopt && 
         hexCharacterSequence.value() == "🐝"
      );

  */    
      return ok;


   }

#ifdef SERVER
   inline bool testWebRequest()
   {
      using namespace BeeFishJSON;

      cout << "Test request" << endl;
      
      bool ok = true;
      
      
      BeeFishWeb::WebRequest requestHeadersOnly;
      ok &= testFile(
         "WebRequest with only headers",
         "server/web-request/tests/request.txt",
         requestHeadersOnly,
         true
      );

      ok &= testResult(
         "WebRequest get",
         requestHeadersOnly.method() == "GET"
      );
      
      ok &= testResult(
         "WebRequest has no json",
         requestHeadersOnly.hasJSON() == false
      );
      
      BeeFishWeb::WebRequest requestBody;
      ok &= testFile(
         "WebRequest with body",
         "server/web-request/tests/request-body.txt",
         requestBody,
         true
      );
      
      ok &= testResult(
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

      ok &= testFile(
         parser,
         "WebRequest with full json",
         "server/web-request/tests/request-full.txt",
         requestFull,
         true
      );
      
      ok &= testResult(
         "WebRequest full has json",
         requestFull.hasJSON() == true
      );

      ok &= testResult(
         "WebRequest full has name",
         hit == true
      );
      
      ok &= testResult(
         "WebRequest full name is Brett",
         name == "Brett"
      );
      
      
      BeeFishMisc::optional<BString> name2;

      BeeFishWeb::WebRequest request2;
      JSONParser parser2(request2);
      parser2.captureValue("name", name2);

      ok &= testFile(
         parser2,
         "WebRequest with full json 2",
         "server/web-request/tests/request-full.txt",
         request2,
         true
      );
      
      ok &= testResult(
         "WebRequest full has name 2",
         name2.hasValue()
      );
      
      ok &= testResult(
         "WebRequest full name is Brett 2",
         name2 == BString("Brett")
      );

      BeeFishWeb::WebRequest urlWebRequest;
      ok &= testFile(
         "WebRequest with path and query",
         "server/web-request/tests/path.txt",
         urlWebRequest,
         BeeFishMisc::nullopt
      );
      
      ok &= testResult(
         "WebRequest path is /path",
         urlWebRequest.path() == "/path"
      );
      
      ok &= testResult(
         "WebRequest query is query",
         urlWebRequest.query() == "query"
      );
      
      BeeFishWeb::WebRequest escapedUrlWebRequest;
      ok &= testFile(
         "WebRequest with escaped path and query",
         "server/web-request/tests/escaped-path.txt",
         escapedUrlWebRequest,
         BeeFishMisc::nullopt
      );
      
      ok &= testResult(
         "WebRequest escaped path is /path",
         escapedUrlWebRequest.path() == "/path"
      );
      
      ok &= testResult(
         "WebRequest escaped query is query<space>query",
         escapedUrlWebRequest.query() == "query%20query"
      );

      BeeFishWeb::WebRequest postWebRequest;
      
      ok &= testFile(
         "Post with encoded name value pairs",
         "server/web-request/tests/post.txt",
         postWebRequest,
         true
      );

      BeeFishWeb::WebRequest postWebRequest2;
      BString body;
      postWebRequest2.setOnData(
         [&body](const BeeFishBString::Data& data) {
            body = BString((const char*)data._data, data.size());
         }
      );

      // Post with anything
      ok &= testFile(
         "Post with content length text",
         "server/web-request/tests/request-text.txt",
         postWebRequest2,
         true
      );

      postWebRequest2.flush();
      

      ok &= testResult(
         "Post with hello world body",
         body == "<h1>Hello world</h1>"
      );

      // Post with anything
      BeeFishWeb::WebRequest postWebRequest3;
      ok &= testFile(
         "Post with zero content length",
         "server/web-request/tests/zero-content-length.txt",
         postWebRequest3,
         true
      );

      postWebRequest2.flush();


      return ok;
   }
   
   inline bool testParts()
   {
      
      cout << "Test request" << endl;
      
      bool ok = true;
      
      
      BeeFishWeb::WebRequest request;
      
      ok &= testFile(
         "WebRequest part 0",
         "server/web-request/tests/request-part-0.txt",
         request,
         BeeFishMisc::nullopt
      );
      
      ok &= testFile(
         "WebRequest part 1",
         "server/web-request/tests/request-part-1.txt",
         request,
         BeeFishMisc::nullopt
      );
      
      ok &= testFile(
         "WebRequest part 2",
         "server/web-request/tests/request-part-2.txt",
         request,
         true
      );
      
      ok &= testResult(
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
         [](const Data& buffer) {
         };

      BeeFishWeb::WebRequest request;
      JSONParser parser(request);

      parser.streamValue("image", onimage);

      ok &= testFile(
         parser,
         "WebRequest Image JSON",
         "server/web-request/tests/image-json.txt",
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
      BeeFishBScript::BScriptParser parser(request);
      //BeeFishBScript::BScriptParser parser(request);

      ok &= testFile(
         parser,
         "POST with hello world sample",
         "server/web-request/tests/post-json.txt",
         request,
         true
      );

      ok &= testResult(
         "Post json value = World",
         parser.json()["Hello"] == "World"
      );

      BeeFishWeb::WebRequest request2;
      BeeFishBScript::BScriptParser parser2(request2);
      //BeeFishBScript::BScriptParser parser(request);

      ok &= testFile(
         parser2,
         "POST to settings",
         "server/web-request/tests/post-settings.txt",
         request2,
         true
      );

      ok &- testResult(
         "Post json value = World",
         parser2.json()["sleeping"] == false
      );

      cout << endl;


      return ok;
   }
#endif


}

#endif
