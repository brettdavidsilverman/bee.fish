#ifndef BEE_FISH_SERVER__OPTIONS_APP_H
#define BEE_FISH_SERVER__OPTIONS_APP_H

#include "../Miscellaneous/Miscellaneous.hpp"
#include "../json/json-parser.h"
#include "../Script/Script.hpp"
#include "../web-request/web-request.h"
#include "session.h"
#include "app.h"
#include "authentication.h"


namespace BeeFishHTTPS {
using namespace std;
using namespace BeeFishWeb;

    class OptionsApp : public App {
    public:
        OptionsApp(
            Session* session,
            ResponseHeaders& responseHeaders
        ) : App(session, responseHeaders)
        {
        }


        virtual void handleResponse()
        override
        {
            const BString& method =
                _session->request()->method();
           /* const URL& url =
                _session->request()->url();
                */
            if (method == "OPTIONS") 
            {
                /*
                _responseHeaders.replace(
                    "content-type",
                    "application/json; charset=utf-8"
                );
                */
                
                _responseHeaders.replace(
                    "access-control-allow-methods",
                    "GET, POST, DELETE, OPTIONS"
                );
                
                _responseHeaders.replace(
                    "access-control-allow-headers",
                    "content-type"
                );
                
                _responseHeaders.replace(
                    "access-control-allow-origin",
                    _session->origin()
                );
    
            /*
                BeeFishScript::Object object
                {
                    {"options", (BeeFishScript::String)url}
                };
                */
                _content = "";//object.str();
                _contentLength = 0;
                _serve = App::SERVE_CONTENT;
                _status = 204;
            
            }
            
        }
        
        virtual BString name()
        {
            return "Options app";
        }

        
    };

    

}

#endif
