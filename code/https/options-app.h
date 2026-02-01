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
            const URL& url =
                _session->request()->url();
                
            if (method == "OPTIONS") 
            {
                _responseHeaders.replace(
                    "content-type",
                    "application/json; charset=utf-8"
                );
            
                BeeFishScript::Object object
                {
                    {"options", (BeeFishScript::String)url}
                };
                
               _content = object.str();
                _serve = App::SERVE_CONTENT;
                _status = 200;
            }
            
        }
        
        virtual BString name()
        {
            return "Options app";
        }

        
    };

    

}

#endif
