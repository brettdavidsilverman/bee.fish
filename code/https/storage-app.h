#ifndef BEE_FISH_SERVER__STORAGE_APP_H
#define BEE_FISH_SERVER__STORAGE_APP_H

#include "../Miscellaneous/Miscellaneous.hpp"
#include "session.h"
#include "app.h"
#include "authentication.h"
#include "storage.h"
#include "../json/json-parser.h"
#include "../Script/Script.hpp"
#include "../web-request/web-request.h"

using namespace std;
using namespace BeeFishWeb;

namespace BeeFishHTTPS {

   class StorageApp : public App {
   public:
      StorageApp(
         Session* session,
         ResponseHeaders& responseHeaders
      ) : App(session, responseHeaders)
      {
      }


      virtual void handleResponse() {
          
         authenticate();

         if (!authenticated())
         {
            throw std::runtime_error("Not authenticated");
         }
         

         WebRequest* request = _session->request();

         BeeFishBString::BString        path;
         BeeFishScript::ObjectPointer  json;
         BeeFishMisc::optional<BString> key = BeeFishMisc::nullopt;
         BeeFishMisc::optional<Id>      id = BeeFishMisc::nullopt;

         BString contentType;
         bool returnJSON = true;

         // Extract id from the search
         BeeFishWeb::URL::Search& search = request->searchObject();

         if (search.contains("id")) {

            BString searchID = search["id"];
            
            // Test for correct Id
            try {
               id = Id::fromKey(searchID);
            }
            catch (...) {
               id = BeeFishMisc::nullopt;
            }

         }
        
         
         if (search.contains("key")) {
            key = search["key"];
         }
         
         const BString& method = request->method();

         path = request->path();

         _bookmark = userData()[path];

         if (key.has_value())
            _bookmark = _bookmark[key.value()];
         else if (id.has_value()) {
            _bookmark = _bookmark[id.value()];
         }
         else if (method != "DELETE")
            return;
        
         
         SSize contentLength = 0;
         
         if (method == "POST")
         {

            if (request->headers().contains("content-type"))
               contentType = request->headers()["content-type"];
            else
               contentType = "application/json; charset=utf-8";

            Size pageIndex = 0;
            

            WebRequest postRequest(false);
            BeeFishParser::Parser parser(postRequest);
            
            postRequest.setOnData(
               [&pageIndex, &contentLength, this](const std::string& data) {
                  contentLength += data.size();
                  _bookmark[pageIndex++].setData(data);
               }
            );

            

            if (!parseWebRequest(parser)) {
               throw std::runtime_error("Invalid input post to storage-app.h");
            }
            
            postRequest.flush();

            
            if ( contentLength == 0 )
               deleteData();
            else {
               _bookmark["Content length"].setData(contentLength);
               _bookmark["Content type"].setData(contentType);
               _bookmark["Page count"].setData(pageIndex);
            }

            returnJSON = true;
            _status = 200;
            
         }
         else if ( method == "GET" )
         {

            if (_bookmark["Content length"].hasData())
               _bookmark["Content type"].getData(contentType);


            if (contentType.length()) {
               _status = 200;
                _bookmark["Content length"].getData(_contentLength);
               _serve = App::SERVE_DATA;
               returnJSON = false;
            }
            else {
               _status = 404;
               returnJSON = true;
            }

         }
         else if (method == "DELETE") {

            deleteData();
           
            _status = 200;

            returnJSON = true;
         }
   
         if ( _status != 200 )
            return;
            
         if ( returnJSON )
         {
            _responseHeaders.replace(
               "content-type",
               "application/json; charset=utf-8"
            );
         }
         else
         {
            if (contentType.length()) {
               _responseHeaders.replace(
                  "content-type",
                  contentType
               );
            }
            else {
               _responseHeaders.replace(
                  "content-type",
                  "text/plain"
               );
            }

            return;
         }

         BeeFishScript::Object output;
         
         output["method"] = method;
         
         if ( key.has_value() )
         {
            output["key"] = key.value();
         }
         
         if ( id.has_value() )
         {
            output["id"] = id.value().key();
         }
               
         output["response"] = "ok";
                  
         _content = output.str();

         _serve = App::SERVE_CONTENT;

      }
      
      virtual BString name()
      {
         return "Storage app";
      }

   private:
      void deleteData() {
         if ( _bookmark.contains("Page count") &&
              _bookmark["Page count"].hasData() )
         {

            size_t pageCount = _bookmark["Page count"];
            for (size_t page = 0; page < pageCount; ++page)
               _bookmark[page].deleteData();

            _bookmark["Page count"] = (size_t)0;
         }
         _bookmark.clear();
 
      }


   };

   

}

#endif
