#ifndef BEE_FISH_SERVER__JSON_APP_H
#define BEE_FISH_SERVER__JSON_APP_H

#include "../Miscellaneous/Miscellaneous.hpp"
#include "session.h"
#include "app.h"
#include "authentication.h"
#include "file-system-app.h"
#include "../json/json-parser.h"
#include "../Script/Script.hpp"
#include "../web-request/web-request.h"
#include "ParseURL.hpp"

using namespace std;
using namespace BeeFishWeb;

namespace BeeFishHTTPS {

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
                
                throw std::runtime_error("Not authenticated");
            }
            
            
            _responseHeaders.replace(
                "content-type",
                "application/json; charset=utf-8"
            );
            
            _responseHeaders.replace(
              "cache-control",
                FileSystemApp::_noCacheControl
            );
            _serve = App::SERVE_JSON;
            _contentLength = -1;
            
            BString error;
    
            Path userData =
                Authentication::userData();
                /*
            const BString& clientIPAddress = 
                session()->ipAddress();
                */
            const BString& method =
                request()->method();
            BString url =
                request()->path();
                /*
            const BString& search =
                request()->search();
            */
            JSONDatabase* database = _session->database();
            /*
            optional<Path> jsonPath =
                parseURL(
                    userData,
                    error,
                    clientIPAddress,
                    method,
                    url,
                    search
                );
            */
        
                 
            JSONPath origin = database->origin();
            JSONPath start = origin;
            /*
            [
                Authentication::userId()
            ];
            */
            char* str = url.data();
            const char* delims ="/";
            char* token;

            // Get the first token
            token = strtok(str, delims);

            // Loop through all remaining tokens
            while (token != nullptr) 
            {
                BString key(token);
                Index index = -1;
                if (key.size()) {
                    key = key.decodeURI();
                    if (key.isDigitsOnly()) {
                        index = atoi(key.c_str());
                    }
                    else {
                        if (key.startsWith("\"") &&
                            key.endsWith("\"") &&
                            key.substr(
                                1,
                                key.length() - 2
                            ).isDigitsOnly()
                        )
                        {
                            index =
                                atoi(
                                    key.substr(
                                        1,
                                        key.length() - 2
                                    ).c_str()
                                );
                        }
                    }
                        
                    if (index != Index(-1))
                    {
                        if (start.contains(index))
                            start = start[index];
                        else
                            break;
                    }
                    else 
                    {
                        if (start.contains(key) || method == "POST")
                            start = start[key];
                        else
                            break;
                    }
                }
                token = strtok(nullptr, delims);
            }
            
            if (token == nullptr) {
                
                _bookmark = start;
                //jsonPath.value();

                if (method == "GET") {
                    _serve = App::SERVE_JSON;
                    _status = 200;
                }
                else if (method == "POST")
                {
                    // Clear the bookmarks contents
                    //_bookmark.clear();
                    
                    // Stream posted file to
                    // database
                    WebRequest postRequest(true);
                    
                    BeeFishDatabase::JSONPathParser
                        parser(
                            _bookmark,
                            postRequest
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
                    }
                    else {
                        _content = "\"ok\"";
                        _serve = App::SERVE_CONTENT;
                        _status = 200;
                        _statusText = "ok";
                    }
                }
            }
            else
            {
                _content = "undefined";
                _serve = App::SERVE_CONTENT;

                _status = 404;
                _statusText = "Path error";
            }
            

        }
        
        virtual BString name()
        {
            return "JSON app";
        }

        
        
    };

    

}

#endif
