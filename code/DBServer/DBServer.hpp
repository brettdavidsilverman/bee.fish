#ifndef BEE_FISH__WEBDB__DBSERVER_HPP
#define BEE_FISH__WEBDB__DBSERVER_HPP
#include "../Miscellaneous/Miscellaneous.hpp"
#include "../WebServer/WebServer.hpp"
#include "../Database/Database.hpp"
#include "../WebRequest/WebRequest.hpp"
#include "StreamToDB.hpp"
#include "StreamFromDB.hpp"
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

      Path urlPath(const string& url) {
         Path path = jsonPath();

         auto onpath =
         [&path](string segment)
         {
            path = path[segment];
            return true;
         };

         auto urlParser =
           BeeFishWeb::URL(onpath);

         urlParser.read(
            url
         );

         return path;
 
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

         // Read from the client socket
         webRequest.read();

         if (webRequest._result != true)
         {
            syslog(LOG_WARNING, "Error reading from client %s at position %i", ipAddress.c_str(), webRequest._index);
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

         if ( webRequest._url == "/" ) {
            outputHomePage(clientSocket);
            return;
         }

         Path path = urlPath(webRequest._url);
  
         string contentType;

     
         if (webRequest._method == "POST")
         {
            if (webRequest._headers
                 .count("content-type")
                 > 0)
            {
               contentType =
                  webRequest.
                  _headers["content-type"];

               path.setData(contentType);
               outputSuccess(clientSocket);
               return;
            }
         }
         else {
            if (path.hasData())
            {
               path.getData(contentType);
            }
            else {
               output404NotFound(clientSocket);
               return;
            }
         }
         

         stringstream stream;
         size_t size = 0;

         if (path.contains("size") &&
             path["size"].hasData())
         {
            size = path["size"];
         }

         stream <<
            "HTTP/2.0 200 OK" << "\r\n" <<
            "Connection: keep-alive\r\n" <<
            "Content-Type: " <<
               contentType << "\r\n";
         if (size > 0)
            stream << "Content-Length: " <<
               size << "\r\n";

         stream <<
            "\r\n";

         std::string headers =
            stream.str();

         // Send the headers
         webRequest.write(
            headers.c_str(),
            headers.length()
         );

         // Send the body
         if (!path.isDeadEnd())
            streamFromDB(webRequest, path);


      }

      virtual void outputSuccess(int clientSocket)
      {
         stringstream stream;

         string json = "{\"success\": true}";

         size_t size = json.size();

         stream <<
            "HTTP/2.0 200 OK\r\n" <<
            "Content-Type: application/json; charset=utf-8\r\n" <<
            "Connection: keep-alive\r\n" <<
            "Content-Length: " <<
               size << "\r\n" <<
            "\r\n" <<
             json;

         std::string response =
            stream.str();

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
            "HTTP/2.0 404 Not Found\r\n" <<
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
cerr << "DBServer.hpp DBWebRequest create JSON body" << endl;

 
      DBServer::Path path = dbServer()->urlPath(_url);
      path.clear();

      return new
         StreamToDB(
            JSON(),
            path
         );
   };

   Parser* DBWebRequest::createContentLengthBody(
      size_t contentLength
   )
   {
      DBServer::Path path = dbServer()->urlPath(_url);
      path.clear();

      return new
         StreamToDB(
            ContentLength(contentLength),
            path
         );
   };


}

#endif