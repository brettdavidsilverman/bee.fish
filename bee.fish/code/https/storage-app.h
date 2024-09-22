#ifndef BEE_FISH_SERVER__STORAGE_APP_H
#define BEE_FISH_SERVER__STORAGE_APP_H

#include "../misc/optional.h"
#include "session.h"
#include "app.h"
#include "authentication.h"
#include "storage.h"
#include "../json/json-parser.h"
#include "../b-script/b-script.h"
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
         
         if (!authenticated())
            return;

         WebRequest* request = _session->request();

         BeeFishBString::BString        path;
         BeeFishBScript::ObjectPointer  json;
         BeeFishMisc::optional<BString> key = BeeFishMisc::nullopt;
         BeeFishMisc::optional<Id>      id = BeeFishMisc::nullopt;

         BString contentType;
         bool returnJSON = true;

         // Extract id from the query
         BeeFishWeb::WebRequest::URL::Query& query = request->queryObject();

         if (query.contains("id")) {

            BString queryID = query["id"].decodeURI();

            // Test for correct Id
            try {
               id = Id::fromKey(queryID);
            }
            catch (...) {
               id = BeeFishMisc::nullopt;
            }

         }

         if (query.contains("key")) {
            key = query["key"].decodeURI();
         }

         path = request->path();

         _bookmark = userData()[path];

         if (key.hasValue())
            _bookmark = _bookmark[key.value()];
         else if (id.hasValue()) {
            _bookmark = _bookmark[id.value()];
         }
         else
             return;

         const BString& method = request->method();

         if (method == "POST")
         {

            if (request->headers().contains("content-type"))
               contentType = request->headers()["content-type"];
            else
               contentType = "text/plain; charset=utf-8";

            size_t pageIndex = 0;
            size_t _contentLength = 0;

            WebRequest postRequest;

            postRequest.setOnData(
               [&pageIndex, &_contentLength, this](const Data& data) {
                  _contentLength += data.size();
                  _bookmark[pageIndex++] = data;
               }
            );

            BeeFishBScript::BScriptParser parser(postRequest);

            std::cerr << "Parsing " << contentType << std::endl;

            if (!parseWebRequest(parser)) {
               throw std::runtime_error("Invalid input post to storage-app.h");
            }

            postRequest.flush();

            if ( _contentLength == 0 )
               deleteData();
            else {
               _bookmark["Content length"] = _contentLength;
               _bookmark["Content type"]   = contentType;
               _bookmark["Page count"]     = pageIndex;
            }

            returnJSON = true;
            _status = 200;

            std::cerr << " OK" << std::endl;
                     
         }
         else if ( method == "GET" )
         {

            if (_bookmark["Content length"].hasData())
               contentType = _bookmark["Content type"];

            if (contentType.length()) {
               _status = 200;
               _contentLength = _bookmark["Content length"];
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
               "application/json; charset=UTF-8"
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

         BeeFishBScript::Object output;
         
         output["method"] = method;
         
         if ( key.hasValue() )
         {
            output["key"] = key.value();
         }
         
         if ( id.hasValue() )
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
