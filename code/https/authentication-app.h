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

         _status = -1;
         
         WebRequest* request = _session->request();
         
         if (request->result() == nullopt)
         {
             request->eof(_session->parser());
             if (request->result() != true)
                 return;
         }

         BString path = request->path();
         BString webMethod = request->method();

         if (path == "/authenticate")
         {
    
            BeeFishMisc::optional<BString> method;
            BeeFishMisc::optional<BString> secret;

            if (webMethod == "GET")
            {
               method = "getStatus";
            }
            else if (webMethod == "POST")
            {
               WebRequest request(true);
               JSONParser parser(request);
               parser.captureValue("method", method);
               parser.captureValue("secret", secret);

               parseWebRequest(parser);

            }
         
            if ( method.has_value() )
            {

               if ( method.value() == "getStatus" )
               {
                  authenticate();
                  _status = 200;
               }
               else if ( method.value() == "logon" && secret.has_value() )
               {
                  logon(
                     secret.value()
                  );
                  _status = 200;
               }
               else if ( method.value() == "logoff" )
               {
                  logoff();
                  _status = 200;
               }
               else
               {
                  return;
               }
            }
            else
            {
               return;
            }
         }
         else
         {
            authenticate();
         }
        
         if (authenticated())
         {
            _responseHeaders.emplace(
               "set-cookie",
               BString("sessionId=") +
               _sessionId +
              BString(";path=/;max-age=3600;secure=true;httponly=false;samesite=None;")
/*
            "secure" => true, // Recommended for production
            "httponly" => false, // Recommended for security
            "samesite" => "None" // Can be 'Strict', 'Lax', or 'None'
*/
        
              
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
         
         if (_status != -1) 
         {
            _responseHeaders.replace(
               "content-type",
               "application/json; charset=utf-8"
            );
         
            BeeFishScript::Object output;

            Authentication
               ::write(output);
            
            _serve = App::SERVE_CONTENT;

            _content = output.str();
         }

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