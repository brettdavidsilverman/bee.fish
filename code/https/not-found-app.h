#ifndef BEE_FISH_SERVER__NOT_FOUND_APP_H
#define BEE_FISH_SERVER__NOT_FOUND_APP_H

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

   class NotFoundApp : public App {
   public:
      NotFoundApp(
         Session* session,
         ResponseHeaders& responseHeaders
      ) : App(session, responseHeaders)
      {
      }


      virtual void handleResponse()
      override
      {
         _responseHeaders.replace(
            "content-type",
            "application/json; charset=utf-8"
         );
         
         BeeFishScript::Object object
         {
            {"error", "Not found"}
         };
            
         _content = object.str();
         _serve = App::SERVE_CONTENT;
         
         _status = 404;
         _statusText = "Not found";
         
      }
      
      virtual BString name()
      {
         return "Not found app";
      }

      
   };

   

}

#endif
