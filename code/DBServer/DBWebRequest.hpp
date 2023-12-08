#ifndef BEE_FISH__WEB_DB__WEB_REQUEST_HPP
#define BEE_FISH__WEB_DB__WEB_REQUEST_HPP

#include <iostream>
#include "StreamFromDB.hpp"
#include "../WebServer/WebServer.hpp"
#include "../WebRequest/WebRequest.hpp"
#include "../DBServer/JSONParser.hpp"

extern "C" uint8_t _binary_NotFound_html_start[];
extern "C" uint8_t _binary_NotFound_html_end[];

extern "C" uint8_t _binary_HomePage_html_start[];
extern "C" uint8_t _binary_HomePage_html_end[];

extern "C" uint8_t _binary_ErrorPage_html_start[];
extern "C" uint8_t _binary_ErrorPage_html_end[];


namespace BeeFishDBServer {

   using namespace std;
   using namespace BeeFishMisc;
   using namespace BeeFishParser;
   using namespace BeeFishJSON;
   using namespace BeeFishWeb;
   using namespace BeeFishWebDB;

   class DBServer;

   class DBWebRequest : public WebRequest {
   protected:
      DBServer* _webServer {nullptr};
      std::mutex _mutex;
      JSONPath _path;
   public:

      DBWebRequest(DBServer* dbServer) :
         WebRequest((WebServer*)dbServer),
         _webServer(dbServer),
         _path(Path(*(Database*)dbServer))
      {
      }

      DBWebRequest(
         DBServer* webServer,
         int socket,
         const std::string& ipAddress
      )
      :   WebRequest(
             (WebServer*)webServer,
             socket,
             ipAddress
          ),
          _webServer(webServer),
          _path(Path(*(Database*)webServer))
      {
      }

      DBWebRequest(const DBWebRequest& source)
      :
         WebRequest(source),
         _webServer(source._webServer),
         _path(source._path)
      {
      }

      virtual ~DBWebRequest() {
      }


      virtual Parser* copy() const
      override
      {
         return new DBWebRequest(*this);
      }
      
      // Defined in DBServer.hpp
      Path root();

      virtual bool setContentType(Path path)
      {

         std::scoped_lock lock(_mutex);

         if (path.hasData())
            return false;

         string contentType;
         if (_headers.count("content-type") > 0)
            contentType = _headers["content-type"];
         
         if (contentType == "")
            contentType = "text/plain; charset=utf-8";

         path.setData(contentType);

         return true;
      }

      virtual Parser* createJSONBody()
      override
      {
         //Path path = root();

         if (!setContentType(_path))
            return nullptr;

         return new
            StreamToDB(
//               JSONParser(path["JSON"], _contentLength),
               JSONParser(_path, _contentLength),
               _path["document"],
               _contentLength
            );
      }

      virtual Parser* createContentLengthBody()
      override
      {
         if (!setContentType(_path))
            return nullptr;

         return new
            StreamToDB(
               ContentLength(_contentLength),
               _path["document"],
               _contentLength
            );
      };


      virtual bool process()
      {
         bool success = true;

         _path = root();

         WebRequest::read();

         stringstream logStream;
         logStream 
               << _ipAddress << " "
               << _method << " "
               << _url << " "
               << _version << endl;

         logMessage(LOG_NOTICE, logStream.str());
         
         if (_method == "POST" &&
             _body == nullptr)
             
         {
            outputFail("Already taken");
            return false;
         }

         if (_result != true) {

            stringstream stream;
            stream << "Invalid content from " << _ipAddress;
            logMessage(LOG_WARNING, stream.str());
            
            if (_method == "POST") {
                
               if (_url.length())
                  _path.deleteData();
                  
               return outputFail(getErrorMessage());
            }
            //else
            //   return outputError();

            return false;
            
         }

         if ( _method == "GET" &&
              _url == "/" )
         {
            return outputHomePage();
         }

         if (_method == "POST")
         {
            return outputSuccess();
         }

         
         if (!_path.hasData() ||
             _path.isDeadEnd())
         {
            return output404NotFound();
         }
         

         // Send the body
         if (_result == true)
         {
            if (streamFromDB(*this, _path, true))
               return true;
         }

         return outputError();

      }

      virtual bool outputJSON(bool success, std::string reason = "")
      {
         stringstream stream;

         stringstream jsonStream;
         jsonStream
            << "{"
               << "\"success\""
               << ":";

         if (success)
            jsonStream << "true";
         else
            jsonStream << "false";

         if (reason.size()) {
            jsonStream
               << ","
               << "\"reason\""
               << ":"
               << "\""
                  << escape(reason)
               << "\"";
         }

         jsonStream << "}";

         string json = jsonStream.str();

         size_t contentLength = json.size();

         stream <<
            "HTTP/2.0 200 OK\r\n" <<
            "Content-Type: application/json; charset=utf-8\r\n" <<
            "Connection: keep-alive\r\n" <<
            "Content-Length: " <<
               contentLength << "\r\n" <<
            "\r\n" <<
             json;

         std::string response =
            stream.str();

         ssize_t written = ::write(
            _socket,
            response.c_str(),
            response.length()
         );
         
         return written == response.length();
         

      }

      virtual bool outputFail(const std::string& reason)
      {
         return outputJSON(
            false,
            reason
         );
      }

      virtual bool outputSuccess()
      {
         return outputJSON(true);
      }

      virtual bool output404NotFound()
      {

         stringstream writeOutput;

         const char * html = (const char *)(&_binary_NotFound_html_start[0]);

         const size_t size =
            _binary_NotFound_html_end -
            _binary_NotFound_html_start;
            
         writeOutput <<
            "HTTP/2.0 404 Not Found\r\n" <<
            "Content-Type: text/html; charset=utf-8\r\n" <<
            "Connection: keep-alive\r\n" <<
            "Content-Length: " <<
               size << "\r\n" <<
            "\r\n";

         std::string headers =
            writeOutput.str();

         ssize_t written;
         
         written = ::write(
            _socket,
            headers.c_str(),
            headers.length()
         );
         
         if (written < headers.length())
            return false;
         
         written = ::write(
            _socket,
            html,
            size
         );
         
         return written == size;
      }

      virtual bool outputHomePage()
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

         std::string headers =
            writeOutput.str();

         ssize_t written;
         
         written = ::write(
            _socket,
            headers.c_str(),
            headers.length()
         );
         
         if (written < headers.length())
            return false;
         
         written = ::write(
            _socket,
            html,
            size
         );
         
         return written == size;
      }

      virtual bool outputError()
      {

         stringstream writeOutput;

         const char * html = (const char *)(&_binary_ErrorPage_html_start[0]);

         const size_t size =
            _binary_ErrorPage_html_end -
            _binary_ErrorPage_html_start;

         writeOutput <<
            "HTTP/2.0 500 Error\r\n" <<
            "Content-Type: text/html; charset=utf-8\r\n" <<
            "Connection: keep-alive\r\n" <<
            "Content-Length: " <<
               size << "\r\n" <<
            "\r\n";

         std::string header =
            writeOutput.str();

         ssize_t written;
         
         written = ::write(
            _socket,
            header.data(),
            header.size()
         );
         
         if (written < header.size())
            return false;
            
         written = ::write(
            _socket,
            html,
            size
         );
         
         return written == size;

      }

      virtual std::string getErrorMessage() const
      override
      {
         if (_body) {
            return _body->getErrorMessage();
         }
         else
            return Parser::getErrorMessage();
      }

   };

   
}

#endif