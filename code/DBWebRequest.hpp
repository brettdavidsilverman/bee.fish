#ifndef BEE_FISH__WEB_DB__WEB_REQUEST_HPP
#define BEE_FISH__WEB_DB__WEB_REQUEST_HPP

#include "WebRequest/WebRequest.hpp"

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

      // Implemented in DBServer.hpp
      DBServer* dbServer();

      virtual Parser* createJSONBody() override;

   };

   
}

#endif