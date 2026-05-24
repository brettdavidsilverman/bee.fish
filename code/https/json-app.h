#ifndef BEE_FISH_SERVER__JSON_APP_H
#define BEE_FISH_SERVER__JSON_APP_H

#include "../Miscellaneous/Miscellaneous.hpp"
#include "../json/json-parser.h"
#include "../Script/Script.hpp"
#include "../web-request/web-request.h"
#include "../query/Query.hpp"

#include "session.h"
#include "app.h"
#include "authentication.h"
#include "file-system-app.h"


namespace BeeFishHTTPS {
using namespace std;
using namespace BeeFishWeb;

    class JSONApp : public App {
    public:
        JSONApp(
            Session* session,
            ResponseHeaders& responseHeaders
        ) : App(session, responseHeaders)
        {
        }


        virtual void handleResponse()
        override
        {
            
            authenticate();

            if (!authenticated())
            {
                return;
                //throw std::runtime_error("Not authenticated");
            }
            
            _responseHeaders.replace(
              "cache-control",
                FileSystemApp::_noCacheControl
            );

            _contentLength = -1;
            
            BString error;
    
                /*
            const BString& clientIPAddress = 
                session()->ipAddress();
                */

            const BString& method =
                request()->method();
            URL& url =
                request()->url();

            const BString& origin = _session->origin();
            const BString& host = _session->host();
            
            ScopedDatabase scoped(this);
            
            JSONDatabase& database = scoped;
            JSONPath jsonPath;
            
            try {
                jsonPath = JSONPath::fromString(
                    *this,
                    database,
                    host,
                    url,
                    method
                );

                _bookmark = jsonPath.index();
            }
            catch (JSONPath::PathNotFoundException& exception)
            {
                return;
            }
            
            if (method == "GET" && 
                request()->search().length())
            {
                _responseHeaders.replace(
                    "content-type",
                    "application/json; charset=utf-8"
                );
            
                _serve = App::SERVE_QUERY;
                _status = 200;


                return;
            }
            else if (method == "GET") {

                // Check for HTTP field
                // This is a special case for serving
                // HTTP content
                if (jsonPath.contains("{HTTP}") &&
                    jsonPath["{HTTP}"].contains("content-type") &&
                    jsonPath["{HTTP}"]["content-type"].type() == Type::STRING)
                {
                    
    
                    BString contentType = 
                    jsonPath
                        ["{HTTP}"]
                        ["content-type"]
                        .getString();
                        
                    _responseHeaders.replace(
                        "content-type",
                        contentType
                    );
                    
                    if (jsonPath["{HTTP}"].contains("content-length") &&
                        jsonPath["{HTTP}"]["content-length"].type() == Type::INTEGER)
                    {
                        _contentLength =
                            jsonPath
                            ["{HTTP}"]
                            ["content-length"]
                            .getInteger();
                        
                    }
                    else
                        _contentLength = -1;
                        
                    _serve = App::SERVE_HTTP;

                }
                else {
                    _responseHeaders.replace(
                        "content-type",
                        "application/json; charset=utf-8"
                    );
                    _serve = App::SERVE_JSON;
                }
                
                _status = 200;
                return;
            }
            else if (method == "POST")
            {

                
                _responseHeaders.replace(
                    "content-type",
                    "application/json; charset=utf-8"
                );
                
                // Save the parent
                JSONPath parent = jsonPath.parent();
                
                BString contentType;
                if (request()->headers().contains("content-type"))
                    contentType = request()->headers()["content-type"];
                else
                    contentType = "text/plain; charset=utf-8";
                    
                if (contentType.startsWith("application/json"))
                {

                    // Posting JSON, remove {HTTP}
                    jsonPath.deleteProperty("{HTTP}");
                    
                    // Stream posted file to
                    // database
                    WebRequest postRequest(true);
                    
                    BeeFishDatabase::JSONPathParser
                        parser(
                            *this,
                            jsonPath,
                            postRequest,
                            clog
                        );

                    if (!parseWebRequest(parser)) {
                        _content = BString("\"") + 
                            parser.getError().escape() +
                            BString("\"");
                        _serve = App::SERVE_CONTENT;
                        _status = 500;
                        _statusText = "JSONPathParser error";
                        return;
                        
                    }
                    
                    if (jsonPath.type() == Type::UNDEFINED)
                    {
                        jsonPath = parent;
                    }
                }
                else {

                    JSONPath http = jsonPath["{HTTP}"];
    
                    JSONPath content = 
                        http["content"];
                    
                    WebRequest postRequest(false);
                    BeeFishParser::Parser parser(postRequest);
                    
                    Index contentLength = 0;
                    Index pageIndex = 0;
                    BString text;
                    
                    bool indexData = contentType.startsWith("text");

                    PagedStream pagedStream(
                        [&content, &contentType, &pageIndex](const BString& encoded)
                        {
                            ++pageIndex;
                            
                            if (pageIndex == 1)
                            {
                                BString header = 
                                    BString("data:") + 
                                    contentType +
                                    BString(";base64,") +
                                    encoded;
                                content.setString(header, pageIndex, false, false);
                            }
                            else 
                            {
                                content.setString(encoded, pageIndex, false, false);
                            }
                        }
                    );
                    
                    BeeFishMisc::Base64EncodeStream
                        base64(pagedStream);
                        
                    postRequest.setOnData(
                        [&pageIndex, &contentLength, &contentType, &content, &base64](const BString& data) {
                            contentLength += data.size();

                            if (contentType.startsWith("text"))
                            {
                                content.setString(data, ++pageIndex, true, false);
                            }
                            else {
                                base64 << data;
                            }
                        }
                    );
                    
                    if (!parseWebRequest(parser)) {
                        throw std::runtime_error("Invalid input post to json-app.h");
                    }

                    postRequest.flush();
                    
                    if (!contentType.startsWith("text"))
                        base64.flush();
                
                    http["content"].endString(pageIndex, indexData);
                    http["content-type"].setString(contentType);
                    http["content-length"].setInteger(contentLength);
                    
                    
                }
                
                _content = BString("\"") + jsonPath.toString(*this).escape() + BString("\"");
                _serve = App::SERVE_CONTENT;
                _status = 200;
                _statusText = "ok";
                return;
                    
            }
            
            assert(false);
            
        }
        
        virtual BString name()
        {
            return "JSON app";
        }
        
    };

    

}

#endif
