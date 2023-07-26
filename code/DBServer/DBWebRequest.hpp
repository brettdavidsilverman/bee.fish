#ifndef BEE_FISH__WEB_DB__WEB_REQUEST_HPP
#define BEE_FISH__WEB_DB__WEB_REQUEST_HPP
#include "StreamFromDB.hpp"
#include "JSONParser.hpp"
#include "../WebRequest/WebRequest.hpp"

extern "C" uint8_t _binary_404_html_start[];
extern "C" uint8_t _binary_404_html_end[];

extern "C" uint8_t _binary_HomePage_html_start[];
extern "C" uint8_t _binary_HomePage_html_end[];

extern "C" uint8_t _binary_ErrorPage_html_start[];
extern "C" uint8_t _binary_ErrorPage_html_end[];


namespace BeeFishWebDB {

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
   public:
      typedef BeeFishDatabase::Path<Database::Encoding> Path;
      DBWebRequest() : WebRequest()
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
          _webServer(webServer)
      {
      }

      DBWebRequest(const DBWebRequest& source)
      :
         WebRequest(source),
         _webServer(source._webServer)
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
      BeeFishWeb::Path root();
      
      DBWebRequest::Path urlPath(const string& url) {
         DBWebRequest::Path path = root();

         auto onpath =
         [&path](string segment)
         {
            path = path[segment];
            return true;
         };

         auto urlParser =
           BeeFishWeb::URL(onpath);

         urlParser.read(url);

         return path;
 
      }

      DBWebRequest::Path path()
      {
         return urlPath(_url);
      }

      virtual bool setContentType(DBWebRequest::Path path)
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
         Path path = DBWebRequest::path();

         if (!setContentType(path))
            return nullptr;

         return new
            StreamToDB(
               JSONParser(path["JSON"], _contentLength),
               path["document"],
               _contentLength
            );
      };

      virtual Parser* createContentLengthBody()
      override
      {
         Path path = DBWebRequest::path();

         if (!setContentType(path))
            return nullptr;

         return new
            StreamToDB(
               ContentLength(_contentLength),
               path["document"],
               _contentLength
            );
      };


      virtual bool process()
      {
         bool success = true;

         WebRequest::read();

         stringstream logStream;
         logStream 
            << _ipAddress << " "
            << _method << " "
            << _url << " "
            << _version << endl;

         logMessage(LOG_NOTICE, logStream.str());
         
         Path path =
            DBWebRequest::path();

         if (_method == "POST" &&
             _body == nullptr)
             
         {
            outputFail("Already taken");
            return false;
         }

         if (_result != true) {

            stringstream logStream;

            logMessage(LOG_WARNING, "Invalid content from %s", _ipAddress.c_str());
            
            if (_method == "POST") {
               path.clear();
               outputFail("Invalid content");
            }
            else
               outputError();

            close();

            return false;
         }

         if ( _method == "GET" &&
              _url == "/" )
         {
            outputHomePage();
            return true;
         }

         if (_method == "POST")
         {
            outputSuccess();
            return true;
         }

         
         if (!path.hasData() ||
             path.isDeadEnd())
         {
            output404NotFound();
            return true;
         }
         

         // Send the body
         if (_result == true)
         {
            if (streamFromDB(*this, path, true))
               return true;
         }

         outputError();

         return true;
      }

      virtual void outputJSON(bool success, std::string reason = "")
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

         ::write(
            _socket,
            response.c_str(),
            response.length()
         );
         

      }

      virtual void outputFail(const std::string& reason)
      {
         outputJSON(
            false,
            reason
         );
      }

      virtual void outputSuccess()
      {
         outputJSON(true);
      }

      virtual void output404NotFound()
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
            _socket,
            response.c_str(),
            response.length()
         );

      }

      virtual void outputHomePage()
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
            _socket,
            response.c_str(),
            response.length()
         );

      }

      virtual void outputError()
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

         writeOutput.write(
            html,
            size
         );

         std::string response =
            writeOutput.str();

         ::write(
            _socket,
            response.data(),
            response.size()
         );

      }

      

   };

   
}

#endif