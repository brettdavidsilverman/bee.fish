//starts here
#include "WebServer/WebServer.hpp"
#include "Database/Database.hpp"
#include "WebRequest/WebRequest.hpp"


namespace BeeFishWeb {

      void WebServer::handleWebRequest(
         WebServer* webServer,
         int clientSocket,
         std::string ipAddress
      )
      {
#ifdef DEBUG
         std::cerr << "handleRequest(" << clientSocket << ")" << std::endl;
#endif
         
         WebRequest webRequest(
            webServer,
            clientSocket,
            ipAddress
         );

         webRequest.process();

      }

}
