#ifndef BEE_FISH_SERVER__ERROR_APP_H
#define BEE_FISH_SERVER__ERROR_APP_H

#include "../Miscellaneous/Miscellaneous.hpp"
#include "session.h"
#include "app.h"
#include "authentication.h"
#include "../json/json-parser.h"
#include "../Script/Script.hpp"
#include "../web-request/web-request.h"
#include "ParseURL.hpp"

using namespace std;
using namespace BeeFishWeb;

namespace BeeFishHTTPS {

   class ErrorApp : public App {
   protected:
      App* _app;
      std::exception& _exception;
      
   public:
      ErrorApp(
         Session* session,
         ResponseHeaders& responseHeaders,
         App* app,
         std::exception& exception
      ) : App(session, responseHeaders),
         _app(app),
         _exception(exception)
      {
      }


      virtual void handleResponse()
      override
      {
         _responseHeaders.replace(
            "content-type",
            "application/json; charset=utf-8"
         );
         
         stringstream stream;
         stream << _exception.what()
                << " by ";
                
         if (_app)
            stream << _app->name();
         else
            stream << "unknown app";
            
         BString error = stream.str();
         
         BeeFishScript::Object object
         {
            {"error", error}
         };
            
         _content = object.str();
         _serve = App::SERVE_CONTENT;
         
         _status = 500;
         _statusText = "Internal Error";
         
      }
      
      virtual BString name()
      override
      {
         return "Error app";
      }

      
   };

   

}

#endif
