#ifndef BEE_FISH_WEB__TEST_REQUEST_H
#define BEE_FISH_WEB__TEST_REQUEST_H

#include "../test/test.h"

#include "../json/json-parser.h"
#include "web-request.h"
#include "../Script/Script.hpp"
#include "../Id/Id.hpp"
#include "../Miscellaneous/Miscellaneous.hpp"

namespace BeeFishWeb
{
#ifdef SERVER
using namespace std::filesystem;
#endif

using namespace BeeFishTest;

    inline bool testURL();
    inline bool testFirstLine();
    inline bool testHeaders();
    inline bool testRequests();
#ifdef SERVER
    inline bool testWebRequest();
    inline bool testParts();
    inline bool testStreams();
    inline bool testJSON();
#endif

    inline bool testRequest()
    {
    
        bool ok = true;

        ok = ok && testURL();
        ok = ok && testFirstLine();
        ok = ok && testHeaders();
        ok = ok && testRequests();
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

        BeeFishWeb::URL::Path path;
        JSONParser pathParser(path);
        pathParser.read("Hello%20World%25");
        pathParser.eof();

        ok = ok && testResult("Path with escaped space is \"Hello World%\"",
            path.result() == true &&
            path.value().decodeURI() == "Hello World%"
        );
        
        ok = ok && testResult("Path with escaped space is \"Hello World%\"",
            path.result() == true &&
            path.value().decodeURI() == "Hello World%"
        );

        BeeFishWeb::URL url;
        JSONParser urlWithQueryParser(url);
        
        if (ok) {
            cout << "Path with three keys" << endl;
            urlWithQueryParser.read("/beehive/settings/?key1=value1&key2=value2&key3");
            urlWithQueryParser.eof();
        }
            
        ok = ok && testResult(
            "Path with search",
            url.search().result() == true
        );

        ok = ok && testResult(
            "Path with search value",
            url.search().value() == "key1=value1&key2=value2&key3"
        );


        ok = ok && testResult(
            "Path with search key 1",
            url.search()["key1"] == "value1"
        );

        ok = ok && testResult(
            "Path with search key 2",
            url.search()["key2"] == "value2"
        );


        ok = ok && testResult(
            "Path with search key 3",
            url.search().contains("key3") && 
            url.search()["key3"].length() == 0
        );

        BeeFishWeb::URL url2;
        JSONParser urlWithQueryParser2(url2);
        urlWithQueryParser2.read("/beehive/settings/?key=hello%20world HTTP/1.1");
        urlWithQueryParser2.eof();
        
        ok = ok && testResult(
            "Path with escaped search",
            url2.search()["key"] == "hello world"
        );
        
        BeeFishWeb::URL url3;
        JSONParser urlWithQueryParser3(url3);
        urlWithQueryParser3.read("/beehive/settings?key=hello%20world HTTP/1.1");
        urlWithQueryParser3.eof();
        
        ok = ok && testResult(
            "Path with escaped search and no slash",
            url3.search()["key"] == "hello world"
        );
        
        BeeFishWeb::URL url4;
        JSONParser urlWithQueryParser4(url4);
        urlWithQueryParser4.read("/beehive?key HTTP/1.1");
        urlWithQueryParser4.eof();
        
        ok = ok && testResult(
            "Path with short search",
            url4.search().contains("key") &&
            url4.search()["key"] == ""
        );
        
        {
            BeeFishWeb::WebRequest::FirstLine line;
            JSONParser parser(line);
            parser.read("GET /beehive?key=%26 HTTP/1.1\r\n");
            parser.eof();
        
            ok = ok && testResult(
                "Path with &",
                line.matched()
            );
            
            ok = ok && testResult(
                "Path with key",
                line.url().search().contains("key")
            );
            
            ok = ok && testResult(
                "Path key is &",
                line.url().search()["key"] == "&"
            );
            
            cerr << "*" << line.url().search()["key"] << "*" << endl;
            assert(ok);
        }
        
        assert(ok);

        return ok;


    }
    
    inline bool testFirstLine() {
        cout << "Test First line" << endl;

        bool ok = true;

        auto test =
        [](string label, string input, string expectedURL)
        {
            
            cerr << "\t" << label << ":";
            
            WebRequest::FirstLine firstLine;

            JSONParser parser(firstLine);
            parser.read(input);
            parser.eof();
            
            bool ok =
               parser.result() == true;
               
            BeeFishMisc::outputSuccess(ok);
            
            if (ok)
                ok = ok && testResult(
                    label + " url",
                    firstLine.fullURL() == expectedURL
                );
                
            if (!ok) {
                cout  << "Result: " 
                     << parser.result() << endl
                     << parser.getError()
                     << endl
                     << "URL parsed: {"
                     << escape(firstLine.fullURL()) << "}" << endl
                     << "URL expected: {"
                     << escape(expectedURL)
                     << "}"
                     << endl;
            }
            
            return ok;
        };
        
        ok = ok && test(
            "Simplest get",
            "GET / HTTP/1.1",
            "/"
        );
        
        ok = ok && test(
            "Correct get",
            "GET / HTTP/1.1\r\n",
            "/"
        );
        
        ok = ok && test(
            "Correct get with full path",
            "GET /test/test?key=value&key2= HTTP/1.1\r\n",
            "/test/test?key=value&key2="
        );
        
        return ok;
    }
    
    inline bool testHeaders()
    {
        using namespace BeeFishJSON;

        cout << "Test request" << endl;
        
        bool ok = true;
        
        Header header;
        JSONParser parser(header);
        parser.read("my-header: boo\r\n");
        parser.eof();

        ok = ok && testResult("Single parser",
            parser.result() == true
        );
        
        ok = ok && testResult("Single header",
            header.result() == true
        );
        
        cout << "*" << header._name << "*" << endl;
        cout << "*" << header._value << "*" << endl;
        cout << "*" << header._value[3] << "*" << endl;

        ok = ok && testResult("Header my-header",
            header._name == "my-header"
        );
        
        ok = ok && testResult("Header value boo",
            header._value == "boo"
        );
    
        
        Headers headers;
        JSONParser parser2(headers);
        parser2.read("my-header1: boo\r\n;my-header2: booboo\r\n\r\n");
        parser2.eof();
        
        cout << "Headers result: " << headers.result() << endl;
        
        ok = ok && testResult("Headers parsed",
            parser.result() == true
        );
        
        ok = ok && testResult("Headers parsed",
            headers.result() == true
        );
        
        ok = ok && testResult("Headers matched",
            headers.matched()
        );
        
        ok = ok && testResult("Headers count",
            headers.size() == 2
        );
        
        return ok;
    }
    
    inline bool testRequests() {
        
        using namespace BeeFishId;
        
        cout << "Test requests" << endl;
        bool ok = true;
        
        auto testRequest =
        [](string label, string request, bool parseJSON = true, bool testHeaders = false, bool testBody = false)
        {
            cout << "\t" << label << ":" << endl;
            
            WebRequest webRequest(parseJSON);
            JSONParser parser(webRequest);
            parser.read(request);
            parser.eof();
    
            
            bool ok = webRequest.result() == true;
            BeeFishMisc::outputSuccess(ok);

            if (ok && testHeaders)
                ok = testResult(
                    label + " headers",
                    webRequest._headers->result() == true
                );
                
            if (ok && testBody)
                ok = testResult(
                    label + " body",
                    webRequest._body->result() == true
                );
            
            if (!ok) {
                cout << "Parser: "
                     << parser.result() << endl;
                     
                cout << "Headers: ";
                
                if (webRequest._headers) 
                     cout << webRequest._headers->result() << endl;
                else
                     cout << " null ptr" << endl;
                     
                cout << parser.getError() << endl;
            }
            
            return ok;
        };

        ok = ok && testRequest(
            "get simplest",
            
            "GET / HTTP/1.1"
        );
        
        
        ok = ok && testRequest(
            "get next simplest",
            
            "GET /sample/ HTTP/1.1\r\n"
        );
        
        ok = ok && testRequest(
            "get simplest correct",
            
            "GET /sample/ HTTP/1.1\r\n"
            "\r\n"
        );

        ok = ok && testRequest(
            "get with simplest header",
            
            "GET /sample/ HTTP/1.1\r\n"
            "Host: bee.fish",
            false,
            true
        );

        ok = ok && testRequest(
            "get with next simplest header",
            
            "GET /sample/ HTTP/1.1\r\n"
            "Host: bee.fish\r\n",
            false,
            true
        );
        
        ok = ok && testRequest(
            "get with simplest correct header",
            "GET /sample/?key= HTTP/1.1\r\n"
            "Host: bee.fish\r\n"
            "\r\n",
            false,
            true
        );
        
        ok = ok && testRequest(
            "get with simplest incorrect header",
            "GET /sample/?key HTTP/1.1\r\n"
            "Host: bee.fish\r\n"
            "\r\n",
            false,
            true
        );


        ok = ok && testRequest(
            "post",
            
            "POST /sample/ HTTP/1.1\r\n"
            "Host: bee.fish\r\n"
            "\r\n"
            "{}",
            true,
            true,
            true
        );
        
        
        ok = ok && testRequest(
            "post with trailing blanks",
            
            "POST /sample/ HTTP/1.1\r\n"
            "Host: bee.fish\r\n"
            "\r\n"
            "{}\r\n",
            true,
            true,
            true
        );

        ok = ok && testRequest(
            "post full",
            
            "POST /sample/ HTTP/1.1\r\n"
            "Host: bee.fish\r\n"
            "\r\n"
            "{\"method\":\"logon\",\"name\":\"Brett\",\"secret\":\"XATG4YZdKVn145VtbCZNl0DcNu/uh/UtnbebTOsSkfUHemzptLOb8lTYmo6JnhLhovGtgXXO1KSeXYIyRfsTtw==\"}",
            true,
            true,
            true
        );
        
        ok = ok && testRequest(
            "post content-length",
            
            "POST /sample/ HTTP/1.1\r\n"
            "Host: bee.fish\r\n"
            "Content-length: 4\r\n"
            "\r\n"
            "1234",
            true,
            true,
            true
        );
        
        
        
        if (ok)
        {
            string request =
            "GET /sample?id=boo HTTP/1.1\r\n"
            "Host: bee.fish\r\n"
            "\r\n";
            WebRequest webRequest(false);
            JSONParser parser(webRequest);
            parser.read(request);
            parser.eof();
    
            ok = ok && testResult(
                "get with search",
                webRequest.result() == true
            );
            
            ok = ok && testResult(
                "search has id",
                webRequest.searchObject().contains("id")
            );
            
            ok = ok && testResult(
                "search has id=boo",
                webRequest.searchObject()["id"] == "boo"
            );
            
            if (!ok)
                cout << parser.getError() << endl;
            
        };
        
        if (ok)
        {
            string request =
            "GET /client/storage/?id=/jHJPjHicH4nI+Jxk+Jw+Jk+JPhycZPhycY+HJxPhycPhyPhxk+HD4PJxjycTyY8ORA= HTTP/1.1";
            WebRequest webRequest(false);
            JSONParser parser(webRequest);
            parser.read(request);
            parser.eof();
    
            ok = ok && testResult(
                "get with full id in search",
                webRequest.result() == true
            );
            
            ok = ok && testResult(
                "search has full id",
                webRequest.searchObject().contains("id")
            );
            
            
            Id id = Id::fromKey(fromBase64(webRequest.searchObject()["id"]));
            
            ok = ok && testResult(
                "search has id=*",
                 id.key() ==  fromBase64("/jHJPjHicH4nI+Jxk+Jw+Jk+JPhycZPhycY+HJxPhycPhyPhxk+HD4PJxjycTyY8ORA=")
            );
            
            if (!ok)
                cout << parser.getError() << endl;
            
        };
        
        if (ok)
        {
            string request =
            "GET /sample?search HTTP/1.1\r\n"
            "Host: bee.fish\r\n"
            "\r\n";
            WebRequest webRequest(false);
            JSONParser parser(webRequest);
            parser.read(request);
            parser.eof();
            
    
            ok = ok && testResult(
                "get with search",
                webRequest.result() == true
            );
            
            ok = ok && testResult(
                "search is search",
                webRequest.search() == "search"
            );
            
            ok = ok && testResult(
                "search has search",
                webRequest.searchObject().contains("search")
            );
            
            ok = ok && testResult(
                "search is empty",
                webRequest.searchObject()["search"] == ""
            );
            
            
            if (!ok)
                cout << parser.getError() << endl;
            
        };
    
        
        return ok;
        
    }



#ifdef SERVER
    inline bool testWebRequest()
    {
        using namespace BeeFishJSON;

        cout << "Test Web request" << endl;
        
        bool ok = true;
        
        
        BeeFishWeb::WebRequest requestHeadersOnly;
        JSONParser parser;
        
        ok = ok && testFile(
            parser,
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
            "WebRequest result",
            requestHeadersOnly.result() == true
        );
        
        ok = ok && testResult(
            "WebRequest get headers",
            requestHeadersOnly._headers != nullptr
        );
        
        ok = ok && testResult(
            "WebRequest get headers result",
            requestHeadersOnly._headers->result() == true
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
            "WebRequest has body",
            requestBody._body != nullptr
        );
        
        ok = ok && testResult(
            "WebRequest has body matched",
            requestBody._body->matched()
        );
        
        ok = ok && testResult(
            "WebRequest has json",
            requestBody.hasJSON()
        );
        
        BString name;
        bool hit = false;


        BeeFishWeb::WebRequest requestFull;

        parser.invokeValue(
            "name",
            [&name, &hit](const BString& key, JSON& json) 
            {
                name = json.value();
                hit = true;
            }
        );

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
        parser.captureValue("name", name2);

        ok = ok && testFile(
            parser,
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

        BeeFishWeb::WebRequest urlWebRequest(false);
        ok = ok && testFile(
            parser,
            "WebRequest with path and search",
            WWW_ROOT_DIRECTORY "/code/web-request/tests/path.txt",
            urlWebRequest,
            true
        );
        
        ok = ok && testResult(
            "WebRequest path is /path",
            urlWebRequest.path() == "/path"
        );
        
        ok = ok && testResult(
            "WebRequest search is query",
            urlWebRequest.search().contains("query")
        );
        
        BeeFishWeb::WebRequest escapedUrlWebRequest;
        ok = ok && testFile(
            parser,
            "WebRequest with escaped path and search",
            WWW_ROOT_DIRECTORY "/code/web-request/tests/escaped-path.txt",
            escapedUrlWebRequest,
            true
        );
        
        ok = ok && testResult(
            "WebRequest escaped path is /path",
            escapedUrlWebRequest.path() == "/path"
        );
        
        ok = ok && testResult(
            "WebRequest escaped search is query<space>query",
            escapedUrlWebRequest.search() == "query%20query"
        );

        BeeFishWeb::WebRequest postWebRequest;
        
        ok = ok && testFile(
            parser,
            "Post with encoded name value pairs",
            WWW_ROOT_DIRECTORY "/code/web-request/tests/post.txt",
            postWebRequest,
            true
        );

        BeeFishWeb::WebRequest postWebRequest2;
        // Post with anything
        ok = ok && testFile(
            parser,
            "Post with content length text",
            WWW_ROOT_DIRECTORY "/code/web-request/tests/request-text.txt",
            postWebRequest2,
            true
        );

        

        
        // Post with anything
        BeeFishWeb::WebRequest postWebRequest3;
        ok = ok && testFile(
            parser,
            "Post with zero content length",
            WWW_ROOT_DIRECTORY "/code/web-request/tests/zero-content-length.txt",
            postWebRequest3,
            nullopt
        );

        BeeFishWeb::WebRequest postWebRequest4;
        BString body;
        postWebRequest4.setOnData(
            [&body](const std::string& data) {
                body = data;
            }
        );

        // Post with anything
        ok = ok && testFile(
            "Post with content length text",
            WWW_ROOT_DIRECTORY "/code/web-request/tests/request-text.txt",
            postWebRequest4,
            true
        );

        postWebRequest4.flush();
        

        ok = ok && testResult(
            "Post with hello world body",
            body == "{\"hi\":\"world\"}"
        );

        // Post with anything
        BeeFishWeb::WebRequest postWebRequest5;
        ok = ok && testFile(
            "Post with zero content length",
            WWW_ROOT_DIRECTORY "/code/web-request/tests/zero-content-length.txt",
            postWebRequest5,
            true
        );

        postWebRequest5.flush();



        return ok;
    }
    
    inline bool testParts()
    {
        
        cout << "Test request parts" << endl;
        
        bool ok = true;
        
        
        BeeFishWeb::WebRequest request;
        JSONParser parser(request);
        
        ok = ok && testFile(
            parser,
            "WebRequest part 0",
            WWW_ROOT_DIRECTORY "/code/web-request/tests/request-part-0.txt",
            request,
            BeeFishMisc::nullopt
        );
        
        ok = ok && testFile(
            parser,
            "WebRequest part 1",
            WWW_ROOT_DIRECTORY "/code/web-request/tests/request-part-1.txt",
            request,
            BeeFishMisc::nullopt
        );
        
        ok = ok && testFile(
            parser,
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
