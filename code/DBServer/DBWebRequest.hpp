#ifndef BEE_FISH__WEB_DB__WEB_REQUEST_HPP
#define BEE_FISH__WEB_DB__WEB_REQUEST_HPP

#include "../WebRequest/WebRequest.hpp"

namespace BeeFishWebDB {

   using namespace std;
   using namespace BeeFishMisc;
   using namespace BeeFishParser;
   using namespace BeeFishJSON;
   using namespace BeeFishWeb;
   using namespace BeeFishWebDB;

   class DBServer;

   class DBWebRequest : public WebRequest {

   public:
      typedef BeeFishDatabase::Path<Database::Encoding> Path;

      DBWebRequest() : WebRequest()
      {
      }

      DBWebRequest(
         WebServer* webServer,
         int socket,
         const std::string& ipAddress
      )
      : WebRequest(
         webServer,
         socket,
         ipAddress
      )
      {
      }

      DBWebRequest(const DBWebRequest& source)
      :
         WebRequest(source)
      {
      }

      virtual ~DBWebRequest() {
      }

      virtual Parser* copy() const
      override
      {
         return new DBWebRequest(*this);
      }

      virtual void setContentType(DBWebRequest::Path path)
      {
         string contentType;
         if (_headers.count("content-type") > 0)
            contentType = _headers["content-type"];
         else
            contentType = "text/plain; charset=utf-8";

         path.setData(contentType);
      }

      // Implemented in DBServer.hpp
      DBServer* dbServer();
      virtual Parser* createJSONBody() override;
      virtual Parser* createContentLengthBody() override;
   };

   
}

#endif