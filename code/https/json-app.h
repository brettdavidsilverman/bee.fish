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

         if (!authenticated())
         {
            throw std::runtime_error("Not authenticated");
            return;
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
         
         JSONDatabase* database =
            session()->database();
            
         BString error;
   
         const BString& clientIPAddress = 
            session()->ipAddress();
         const BString& method =
            request()->method();
         const BString& url =
            request()->path();
         const BString& query =
            request()->query();
         
         optional<Path> jsonPath =
            parseURL(
               *database,
               error,
               clientIPAddress,
               method,
               url,
               query
            );
         
         if (jsonPath.has_value())
         {
            _bookmark = jsonPath.value();
            if (method == "GET") {
               _serve = App::SERVE_JSON;
               _status = 200;
            }
            else if (method == "POST")
            {
               // Clear the bookmarks contents
               _bookmark.clear();
               
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
                  _content = "\"Ok\"";
                  _serve = App::SERVE_CONTENT;
                  _status = 200;
                  _statusText = "Ok";
               }
            }
         }
         else
         {
            BeeFishScript::Object object
            {
               {"error", error}
            };
            _content = object.str();
            _serve = App::SERVE_CONTENT;
            _status = 404;
            _statusText = "Path error";
         }
         
         

      }
      
      virtual BString name()
      {
         return "JSON app";
      }
/*
      virtual void write(ostream& stream)
      {
         App::write(stream);
         JSONPath path(_bookmark);
         stream << path;
      }
      */
   };

   

}

#endif
