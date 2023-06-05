#ifndef BEE_FISH__WEB__DBSERVER_HPP
#define BEE_FISH__WEB__DBSERVER_HPP
#include "WebServer/WebServer.hpp"
#include "Database/Database.hpp"
#include "WebRequest/WebRequest.hpp"
#include "Version.hpp"


extern "C" uint8_t _binary_404_html_start[];
extern "C" uint8_t _binary_404_html_end[];

extern "C" uint8_t _binary_HomePage_html_start[];
extern "C" uint8_t _binary_HomePage_html_end[];


namespace BeeFishWeb {

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

      Path root() {
         return Path(*this)[host()];
      }


      virtual void handleWebRequest(
         int clientSocket,
         string ipAddress
      ) override
      {
#ifdef DEBUG
         std::cerr << "handleRequest(" << clientSocket << ")" << std::endl;
#endif

         WebRequest webRequest(
            this,
            clientSocket,
            ipAddress
         );

         BeeFishDatabase::Path dbPath = root();

         webRequest.read();

         if (webRequest._result == false)
         {
            cerr << "Error reading from client" << endl;
            return;
         }

         stringstream output;

         auto onpath =
         [&dbPath, &output](string path)
         {
            dbPath << path;
            return true;
         };

         if ( webRequest._url == "/" ) {
            outputHomePage(clientSocket);
            return;
         }

         auto urlParser =
           URL(onpath) and newLine;

         bool success;

         success = urlParser.read(
            webRequest._url + "\r\n"
         );
    
         success = success &&
            urlParser._result != false;

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

}

#endif
