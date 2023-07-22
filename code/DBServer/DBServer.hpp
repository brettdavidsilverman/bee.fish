#ifndef BEE_FISH__WEBDB__DBSERVER_HPP
#define BEE_FISH__WEBDB__DBSERVER_HPP
#include "../Miscellaneous/Miscellaneous.hpp"
#include "../WebServer/WebServer.hpp"
#include "../Database/Database.hpp"
#include "../WebRequest/WebRequest.hpp"
#include "StreamToDB.hpp"

#include "DBWebRequest.hpp"
#include "Version.hpp"

namespace BeeFishWebDB {

   using namespace BeeFishMisc;
   using namespace BeeFishWeb;

   class DBServer :
      public WebServer,
      public Database
   {
   public:
      typedef DBWebRequest::Path Path;

      DBServer(
         string host = WEB_SERVER_HOST,
         int port = WEB_SERVER_PORT,
         int threads = WEB_SERVER_THREADS,
         string databaseFilename = DATABASE_FILENAME)
      :
         WebServer(host, port, threads),
         Database(databaseFilename)
      {
      }

      virtual ~DBServer() {
      }

      
      virtual bool handleWebRequest(
         int clientSocket,
         string ipAddress
      ) override
      {

         DBWebRequest webRequest(
            this,
            clientSocket,
            ipAddress
         );

         // Read from the client socket
         if (!webRequest.process()) {
            return false;
         }

         return true;

      }

   };
   
   // Defined in DBWebRequest.hpp
   BeeFishWeb::Path DBWebRequest::root() {
      Database* database =
         dynamic_cast<Database*>
            (_webServer);

      return BeeFishWeb::Path(*database)
         [_webServer->host()];
   }

}

#endif
