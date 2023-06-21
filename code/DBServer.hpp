#ifndef BEE_FISH__WEB__DBSERVER_HPP
#define BEE_FISH__WEB__DBSERVER_HPP
#include "Miscellaneous/Miscellaneous.hpp"
#include "WebServer/WebServer.hpp"
#include "Database/Database.hpp"
#include "WebRequest/WebRequest.hpp"
#include "StreamToDB.hpp"
#include "DBWebRequest.hpp"
#include "Version.hpp"


extern "C" uint8_t _binary_404_html_start[];
extern "C" uint8_t _binary_404_html_end[];

extern "C" uint8_t _binary_HomePage_html_start[];
extern "C" uint8_t _binary_HomePage_html_end[];


namespace BeeFishWebDB {

   using namespace BeeFishMisc;
   using namespace BeeFishWeb;

   class DBServer :
      public WebServer,
      public Database
   {
   public:
      typedef BeeFishDatabase::Path<Database::Encoding> Path;

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

      Path root() {
         return Path(*this)[host()];
      }

      Path jsonPath() {
         return root()["json"];
      }

      virtual void handleWebRequest(
         int clientSocket,
         string ipAddress
      ) override
      {
         syslog(LOG_DEBUG, "DBServer::handleWebRequest(%s)", ipAddress.c_str() );

         DBWebRequest webRequest(
            this,
            clientSocket,
            ipAddress
         );

         BeeFishDatabase::Path dbPath = root();
         
         // Read from the client socket
         webRequest.read();

         if (webRequest._result != true)
         {
            syslog(LOG_WARNING, "Error reading from client %s", ipAddress.c_str());
            ::close(clientSocket);
            return;
         }

         stringstream logStream;
         logStream 
            << webRequest._ipAddress << " "
            << webRequest._method << " "
            << webRequest._url << " "
            << webRequest._version << endl;

         logMessage(LOG_NOTICE, logStream.str());

         stringstream output;

         if ( webRequest._url == "/" ) {
            outputHomePage(clientSocket);
            return;
         }

         if (webRequest._url[
                webRequest._url.size() - 1
             ] != '/' &&
             webRequest._url.find("?") == 
                std::string::npos
            )
         {
            webRequest._url += "/";
         }

         auto onpath =
         [&dbPath, &output](string path)
         {
            dbPath << path;
            return true;
         };

         auto urlParser =
           URL(onpath);
         bool success = true;

         success = success &&
            urlParser.read(
               webRequest._url
            );

         success = success &&
            (urlParser._result != false);

         string contentType = "text/plain; charset=utf-8";

         if (success) {
            if (webRequest._method == "POST") {
               dbPath["content-type"].setData(
                  webRequest._contentType
               );
               dbPath.setData(webRequest._capture);
               output << webRequest._capture;
            }
            else if (!dbPath.hasData()) {
               output404NotFound(clientSocket);
               return;
            }
            else {
               dbPath["content-type"].getData(contentType);
               string body;
               dbPath.getData(body);
               output << body;
            }
         }
         else {
            output << "Error: " << webRequest._url << endl;
         }

         string out = output.str();

         stringstream writeOutput;

         writeOutput <<
            "HTTP/2.0 200 OK\r\n" <<
            "Content-Type: " << contentType << "\r\n" <<
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

      
      virtual void output404NotFound(int clientSocket)
      {

         stringstream writeOutput;

         const char * html = (const char *)(&_binary_404_html_start[0]);

         const size_t size =
            _binary_404_html_end -
            _binary_404_html_start;

         writeOutput <<
            "HTTP/2.0 404 OK\r\n" <<
            "Content-Type: text/html; charset=utf-8\r\n" <<
            "Connection: keep-alive\r\n" <<
            "Content-Length: " <<
               size << "\r\n" <<
            "\r\n";

         writeOutput.write(
            html,
            size
         );

         std::string response =
            writeOutput.str();

         ::write(
            clientSocket,
            response.c_str(),
            response.length()
         );

      }

      virtual void outputHomePage(int clientSocket)
      {

         stringstream writeOutput;

         const char * html = (const char *)(&_binary_HomePage_html_start[0]);

         const size_t size =
            _binary_HomePage_html_end -
            _binary_HomePage_html_start;

         writeOutput <<
            "HTTP/2.0 200 OK\r\n" <<
            "Content-Type: text/html; charset=utf-8\r\n" <<
            "Connection: keep-alive\r\n" <<
            "Content-Length: " <<
               size<< "\r\n" <<
            "\r\n";

         writeOutput.write(
            html,
            size
         );

         std::string response =
            writeOutput.str();

         ::write(
            clientSocket,
            response.c_str(),
            response.length()
         );

      }

   };

   DBServer* DBWebRequest::dbServer()
   {
      DBServer* dbServer =
         dynamic_cast<DBServer*>(_webServer);

      return dbServer;
   }

   Parser* DBWebRequest::createJSONBody()
   {
      return new Capture(
         StreamToDB(
            JSON(),
            dbServer()->jsonPath()
         ),
         _capture
      );
   };


}

#endif
