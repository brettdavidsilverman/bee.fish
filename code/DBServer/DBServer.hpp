#ifndef BEE_FISH__WEBDB__DBSERVER_HPP
#define BEE_FISH__WEBDB__DBSERVER_HPP
#include "../Miscellaneous/Miscellaneous.hpp"
#include "../Miscellaneous/SigHandler.hpp"

#include "../WebServer/WebServer.hpp"
#include "../Database/Database.hpp"
#include "../WebRequest/WebRequest.hpp"
#include "StreamToDB.hpp"

#include "DBWebRequest.hpp"
#include "Version.hpp"

namespace BeeFishDBServer {

   using namespace BeeFishMisc;
   using namespace BeeFishWeb;

   class DBServer :
      public WebServer,
      public Database
   {
   public:

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
         try {
            DBWebRequest webRequest(
               this,
               clientSocket,
               ipAddress
            );

            // Read from the client socket
            if (!webRequest.process()) {
               ::close(clientSocket);
               return false;
            }
         }
         catch (...)
         {
             stringstream stream;
             logMessage(LOG_NOTICE, "Error processing client %s", ipAddress.c_str());
             ::close(clientSocket);
             return false;
         }

         return true;

      }

   };
   
   // Defined in DBWebRequest.hpp
   Path DBWebRequest::root() {
      Database* database =
         dynamic_cast<Database*>
            (_webServer);

      return BeeFishWeb::Path(*database)
         [_webServer->host()];
   }

}

#endif
