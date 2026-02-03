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

            _contentLength = -1;
            
            BString error;
    
                /*
            const BString& clientIPAddress = 
                session()->ipAddress();
                */
            const BString& method =
                request()->method();
            const URL& url =
                request()->url();
            
            BString host = _session->host();

                /*
            const BString& userId =
                Authentication::userId();
                */
                /*
            const BString& search =
                request()->search();
            */
        
            
            if (method == "GET" && 
                request()->search().length())
            {
cout << __FILE__ << " " << request()->search() << endl;
                _serve = App::SERVE_QUERY;
                _status = 200;
                return;
            }

            ScopedDatabase scoped(this);
            
            JSONDatabase* database = scoped;
            JSONPath jsonPath;
            
            try {
                jsonPath = JSONPath::fromString(
                    *database,
                    host,
                    url,
                    method
                );
                _bookmark = jsonPath.index();
            }
            catch (JSONPath::PathNotFoundException& exception)
            {
                _serve = App::SERVE_CONTENT;
                _content = "undefined";
                _status = 404;
                _statusText = 
                    BString("Not found ") +
                    host + exception.url();
                return;
            }
                
            if (method == "GET") {
                _serve = App::SERVE_JSON;
                _status = 200;
            }
            else if (method == "POST")
            {
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
                }
                else {
                    _content = BString("\"") + BString(host + url).escape() + BString("\"");
                    _serve = App::SERVE_CONTENT;
                    _status = 200;
                    _statusText = "ok";
                }
            }
            
        }
        
        virtual BString name()
        {
            return "JSON app";
        }
        
    };

    

}

#endif
