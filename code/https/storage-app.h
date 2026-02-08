#ifndef BEE_FISH_SERVER__STORAGE_APP_H
#define BEE_FISH_SERVER__STORAGE_APP_H

#include "../Miscellaneous/Miscellaneous.hpp"
#include "../json/json-parser.h"
#include "../Script/Script.hpp"
#include "../web-request/web-request.h"
#include "../Database/Database.hpp"
#include "../Id/Id.hpp"
#include "session.h"
#include "app.h"
#include "authentication.h"



namespace BeeFishHTTPS {
using namespace std;
using namespace BeeFishWeb;
using namespace BeeFishId;

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
             return;
           // throw std::runtime_error("Not authenticated");
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

         ScopedDatabase scoped(this);
         
         JSONDatabase* database = scoped;
        
         Path userData =
                database->userData(_userId);
         Path bookmark = userData[URLS][path];

         if (key.has_value())
            bookmark = bookmark[key.value()];
         else if (id.has_value()) {
            bookmark = bookmark[id.value()];
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
               [&pageIndex, &contentLength, &bookmark, this](const std::string& data) {
                  contentLength += data.size();
                  bookmark[pageIndex++].setData(data);
               }
            );

            

            if (!parseWebRequest(parser)) {
               throw std::runtime_error("Invalid input post to storage-app.h");
            }
            
            postRequest.flush();

            
            if ( contentLength == 0 )
               deleteData(bookmark);
            else {
               bookmark["Content length"].setData(contentLength);
               bookmark["Content type"].setData(contentType);
               bookmark["Page count"].setData(pageIndex);
            }

            returnJSON = true;
            _status = 200;
            
         }
         else if ( method == "GET" )
         {

            if (bookmark["Content length"].hasData())
               bookmark["Content type"].getData(contentType);


            if (contentType.length()) {
               _status = 200;
                bookmark["Content length"].getData(_contentLength);
               _serve = App::SERVE_DATA;
               _bookmark = bookmark.index();
               returnJSON = false;
            }
            else {
               _status = 404;
               returnJSON = true;
            }

         }
         else if (method == "DELETE") {

            deleteData(bookmark);
           
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
      void deleteData(Path bookmark) {
         if ( bookmark.contains("Page count") &&
              bookmark["Page count"].hasData() )
         {

            size_t pageCount = bookmark["Page count"];
            for (size_t page = 0; page < pageCount; ++page)
               bookmark[page].deleteData();

            bookmark["Page count"].setData((size_t)0);
         }
         bookmark.clear();
 
      }


   };

   

}

#endif
