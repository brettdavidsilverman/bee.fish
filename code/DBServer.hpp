#ifndef BEE_FISH__WEB__DBSERVER_HPP
#define BEE_FISH__WEB__DBSERVER_HPP
#include "WebServer/WebServer.hpp"
#include "Database/Database.hpp"
#include "WebRequest/WebRequest.hpp"
#include "Version.hpp"

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

      BeeFishDatabase::Path dbPath(
         webServer->root()
      );

      if (!webRequest.read())
         return;

      std::stringstream output;

      output << webServer->version() << endl;
      output << webServer->_database << endl;
      output << endl;

      string path;
      auto onpath =
      [&path, &dbPath, &output](Parser* parser)
      {
         output << path << endl;
         dbPath << path;
         path = "";
      };

      auto urlParser =
        URL(path, onpath) and newLine;

      bool success = false;
      success = urlParser.read(
         webRequest._url + "\r\n"
      );
    
      success = success &&
         urlParser._result != false;

      if (success) {
         if (dbPath.isDeadEnd()) {
            output << "First 🥇" << endl;
            dbPath << "content-type";
         }
         else
            output << "Second 🥈" << endl;
      }
      else {
         output << "Error: " << webRequest._url << endl;
      }

      std::string out = output.str();

      std::stringstream writeOutput;

      writeOutput <<
         "HTTP/2.0 200 OK\r\n" <<
         "Content-Type: text/plain; charset=utf-8\r\n" <<
         "Connection: keep-alive\r\n" <<
         "Content-Length: " <<
            out.length() << "\r\n" <<
         "\r\n" <<
         out;

      std::string response =
         writeOutput.str();

      ::write(
         clientSocket,
         response.c_str(),
         response.length()
      );


   }

}

#endif
