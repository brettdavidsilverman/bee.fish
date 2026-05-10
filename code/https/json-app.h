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

                /*
            const BString& userId =
                Authentication::userId();
                */
                /*
            const BString& search =
                request()->search();
            */
        
            
            
            ScopedDatabase scoped(this);
            
            JSONDatabase* database = scoped;
            JSONPath jsonPath;
            
            try {
                jsonPath = JSONPath::fromString(
                    *database,
                    origin,
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
            
                
            if (method == "GET") {
                // Check for HTTP field
                // This is a special casw for serving
                // HTTP content
                if (jsonPath.contains("{HTTP}") &&
                    jsonPath["{HTTP}"].contains("content-type") &&
                    jsonPath["{HTTP}"]["content-type"].type() == Type::STRING)
                {
                    _responseHeaders.replace(
                        "content-type",
                        jsonPath
                            ["{HTTP}"]
                            ["content-type"]
                            .getString()
                    );
                    
                    if (jsonPath["{HTTP}"].contains("content-length") &&
                        jsonPath["{HTTP}"]["content-length"].type() == Type::INTEGER)
                    {
                        stringstream stream;
                        _contentLength =
                            jsonPath
                            ["{HTTP}"]
                            ["content-length"]
                            .getInteger();
                            
                        stream << _contentLength;
                        
                        _responseHeaders.replace(
                            "content-length",
                            stream.str()
                        );
                    }
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
            }
            else if (method == "POST")
            {
                
                _responseHeaders.replace(
                    "content-type",
                    "application/json; charset=utf-8"
                );
                
                
                std::vector paths = request()->url().paths();
                BString contentType;
                if (request()->headers().contains("content-type"))
                    contentType = request()->headers()["content-type"];
                else
                    contentType = "text/plain; charset=utf-8";
                    
 
                if (paths.size() > 0 &&
                    paths[paths.size() - 1] == "{HTTP}" &&
                    !contentType.startsWith("application/json"))
                {

                    jsonPath.deleteProperty("content");
                    JSONPath content = jsonPath["content"];
                    
                    WebRequest postRequest(false);
                    BeeFishParser::Parser parser(postRequest);
                    
                    Index contentLength = 0;
                    Size pageIndex = 0;
                    BString text;
                    
                    postRequest.setOnData(
                        [&pageIndex, &contentLength, &contentType, &content, &text](const BString& data) {
                            contentLength += data.size();
                            if (contentType.startsWith("text/"))
                                text += data;
                            else {
                                BString base64 = data.toBase64();
                                content[++pageIndex].setString(base64, false);
                            }
                        }
                    );
                    
                    if (!parseWebRequest(parser)) {
                        throw std::runtime_error("Invalid input post to json-app.h");
                    }

                    postRequest.flush();

                    if ( contentLength == 0 )
                        jsonPath.setUndefined();
                    else {
                        jsonPath["content-type"].setString(contentType);
                        jsonPath["content-length"].setInteger(contentLength);
                        if (contentType.startsWith("text/"))
                            jsonPath["content"].setString(text);
                    }

                }
                else {
                    // Save the parent
                    JSONPath parent = jsonPath.parent();
                    
                    // Posting JSON, remove {HTTP}
                    jsonPath.deleteProperty("{HTTP}");
                    
                    // Stream posted file to
                    // database
                    WebRequest postRequest(true);
                    
                    BeeFishDatabase::JSONPathParser
                        parser(
                            jsonPath,
                            postRequest,
                            clog
                        );

                    if (!parseWebRequest(parser)) {
                        BeeFishScript::Object object
                        {
                            {"error", parser.getError()}
                        };
                        _content = object.str();
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
                
                
                    
                _content = BString("\"") + jsonPath.toString().escape() + BString("\"");
                _serve = App::SERVE_CONTENT;
                _status = 200;
                _statusText = "ok";
                    
            }
            
        }
        
        virtual BString name()
        {
            return "JSON app";
        }
        
    };

    

}

#endif
