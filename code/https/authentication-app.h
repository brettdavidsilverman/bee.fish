#ifndef BEE_FISH_HTTPS__HTTPS_AUTHENTICATION_H
#define BEE_FISH_HTTPS__HTTPS_AUTHENTICATION_H

#include "app.h"
#include "session.h"

namespace BeeFishHTTPS {

   using namespace BeeFishDatabase;
   using namespace BeeFishPowerEncoding;
   using namespace BeeFishHTTPS;
   using namespace BeeFishJSON;
   using namespace BeeFishWeb;
   
   class AuthenticationApp :
      public App
   {
   protected:
      inline static vector<BString>
      
         _privileged
         {
            "/",
            "/authenticate",
            "/index.html",
            "/favicon.ico",
            "/feebee.jpg",
            "/feebee-small.jpg",
            "/head.js",
            "/body.js",
            "/style.css",
            "/client/style.css",
            "/client/fetch.js",
            "/client/evaluate.js",
            "/client/punycode.js",
            "/client/console/console.js",
            "/client/logon/",
            "/client/logon/index.html",
           // "/client/logon/logoff.html",
            "/client/logon/style.css",
            "/client/logon/sha512.js",
            "/client/logon/hash-secret.js",
            "/client/logon/authentication.js",
            "/client/storage/storage.js"
         };

   public:
      AuthenticationApp(
         Session* session,
         ResponseHeaders& responseHeaders
      ) : App(session, responseHeaders)
      {
      
      }

      virtual void handleResponse() {

         WebRequest* request = _session->request();
         
         if (request->result() != true) {
            _status = -1;
            return;
         }
         
         BString path = request->path();
         
         if (path != "/authenticate") {
            _status = -1;
            return;
         }
         
         BString webMethod = request->method();
         
         BeeFishMisc::optional<BString> method;
         BeeFishMisc::optional<BString> secret;

         if (webMethod == "GET")
         {
            method = "getStatus";
         }
         else if (webMethod == "POST")
         {
            WebRequest* request = new WebRequest(true);
            ScriptParser parser(*request);
            
            if (parseWebRequest(parser) &&
                request->hasJSON())
            {

               BeeFishScript::ObjectPointer object =
                  parser.json();

               if (object->contains("method")) {
                  method = (BString&)(*object)["method"];
               }

               if (object->contains("secret"))
                  secret = (BString&)(*object)["secret"];

            }
            delete request;
         }

         if ( method.has_value() )
         {

            _status = 200;
            
            if ( method.value() == "getStatus" )
            {
               authenticate();
            }
            else if ( method.value() == "logon" && secret.has_value() )
            {
               logon(
                  secret.value()
               );
            }
            else if ( method.value() == "logoff" )
            {
               logoff();
            }
            else
            {
               _status = -1;
               return;
            }
         }
         else
         {
            _status = -1;
            return;
         }
      /*
         
         BString origin;
         
         origin = session()->origin();

         _responseHeaders.replace(
            "access-control-allow-origin",
            origin
         );
*/
         _responseHeaders.replace(
            "vary",
            "origin"
         );

         _responseHeaders.replace(
            "access-control-allow-credentials",
            "true"
         );
         
         
         _responseHeaders.replace(
            "connection",
            "keep-alive"
         );

         
         
         if (authenticated())
         {
            _responseHeaders.emplace(
               "set-cookie",
               BString("sessionId=") +
               _sessionId +
              BString(";path=/;max-age=3600;")
            );
         }
         else
         {
            _responseHeaders.emplace(
               "set-cookie",
               "sessionId=;path=/;max-age=0;"
            );
         }


         _responseHeaders.replace(
            "cache-control",
            "no-store"
         );
         
         if ( !authenticated() &&
               !isPrivileged(
                  path,
                  webMethod
               ) )
         {
            _status = 401;
            _statusText = "Not authenticated";
            _serve = SERVE_FILE;
            _filePath = getFilePath("/client/logon/index.html");
            return;
            
         }

         _responseHeaders.replace(
            "content-type",
            "application/json; charset=utf-8"
         );
         
         BeeFishScript::Object output;

         Authentication
            ::write(output);
            
         _serve = App::SERVE_CONTENT;

         _content = output.str();
         _status = 200;
         
      }
      
      bool isPrivileged(const BString& path, const BString& webMethod)
      {
         return
            ( std::find(
                _privileged.begin(),
                _privileged.end(),
                path )
             != _privileged.end() );
      }
      
      virtual BString name()
      {
         return "HTTPS Authentication App";
      }
   };
   
   

};

#endif