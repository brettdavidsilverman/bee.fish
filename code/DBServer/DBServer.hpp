#ifndef BEE_FISH__WEBDB__DBSERVER_HPP
#define BEE_FISH__WEBDB__DBSERVER_HPP
#include "../Miscellaneous/Miscellaneous.hpp"
#include "../Miscellaneous/SigHandler.hpp"

#include "../WebServer/WebServer.hpp"
#include "../Database/Database.hpp"
#include "../WebRequest/WebRequest.hpp"
#include "../WebRequest/URLHandler.hpp"
#include "StreamToDB.hpp"

#include "DBWebRequest.hpp"
#include "Version.hpp"

namespace BeeFishDBServer {

   using namespace BeeFishMisc;
   using namespace BeeFishWeb;
   using namespace BeeFishDatabase;

   class DBServer :
      public Database,
      public WebServer
   {
   public:

      DBServer(
         string host = WEB_SERVER_HOST,
         int port = WEB_SERVER_PORT,
         int threads = WEB_SERVER_THREADS,
         string databaseFilename = ""
      ) :
         Database(databaseFilename),
         WebServer(host, port, threads)
      {
         
         _onsegment =
            [this](string segment) {
               cerr << "DBServer.hpp: " << segment <<  endl;
               return true;
            };

      }

      virtual ~DBServer() {
      }

      Path root() {
         Path root = Path(*this);
         return root[url()];
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
            }
         }
         catch (...)
         {
             stringstream stream;
             stream << "Error processing client " << ipAddress;
             logMessage(LOG_NOTICE, stream.str());
             ::close(clientSocket);
         }

         return true;

      }


   };

   // Declared in DBWebRequest.hpp
   Path DBWebRequest::root() {
      return _webServer->root();
   }

}

#endif
