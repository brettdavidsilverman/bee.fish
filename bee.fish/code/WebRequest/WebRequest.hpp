#ifndef BEE_FISH__WEB__WEB_REQUEST_H
#define BEE_FISH__WEB__WEB_REQUEST_H

#include <map>
#include <vector>
#include <typeinfo>
#include <algorithm>
#include <cctype>
#include <string>
#include <poll.h>

#include "../Miscellaneous/Miscellaneous.hpp"

#include "../Parser/Parser.hpp"
#include "../JSON/JSON.hpp"
#include "../Database/Database.hpp"
#include "Header.hpp"
#include "Config.hpp"

namespace BeeFishWeb {

   using namespace std;
   using namespace BeeFishMisc;
   using namespace BeeFishParser;
   using namespace BeeFishJSON;
   using namespace BeeFishDatabase;

   class WebServer;
   
   typedef std::function<bool (std::string)> OnSegment;

   class WebRequest : public Parser {
   protected:
      int _socket;
   public:
      WebServer* _webServer;
      string _ipAddress;
      string _method;
      string _url;
      string _version;
      map<string, string> _headers;
      size_t _contentLength {0};
      size_t _bytesRead {0};

      Parser* _body = nullptr;
      And _parser = createParser();
      
      size_t _index {0};
      string _segment;
      string _path;
      OnSegment _onsegment;
   public:
       
      WebRequest ()
         :
         _socket(-1),
         _webServer(nullptr)
      {
          _onsegment =
             [this](string segment) {
                _path += "/" + segment;
                return true;
             };
      }
      
      using Parser::read;

      // Defined in WebServer.hpp
      WebRequest(WebServer* webServer, int socket, const std::string& ipAddress);

      WebRequest(const WebRequest& source)
      :
         _webServer(source._webServer),
         _socket(-1),
         _ipAddress(source._ipAddress),
         _onsegment(source._onsegment)
      {
      }

      virtual ~WebRequest() {
         
         close();

      }

      Parser* copy() const override
      {
         return new WebRequest(*this);
      }
/*
      virtual Path root() {
         return Path(*(Database*)_webServer);
      };
*/

      BeeFishParser::And createParser() {
          
         const auto method =
            Capture(
               Word("GET") or
               Word("POST") or
               Word("DELETE"),
               _method
            );
            
         const auto version =
            Capture(
               Word("HTTP/") and
               integer and
               decimal,
               _version
            );
            
         const auto segment =
            Invoke(
               Capture(
                  Character("/") and
                  Repeat(
                     not (
                        Character("/") or
                        Character("?") or
                        blank
                     ),
                     0
                  ),
                  _segment
               ),
               [this](Parser*) {
                   
                  bool success = true;

                  if ( _onsegment &&
                       _segment != "/" )
                  {
                     string segment =
                        _segment.substr(1);

                     success = _onsegment(segment);
                  }

                  _segment = "";
                  return success;
               }
            );
            
         const auto query =
            Character("?") and
            Repeat(
                not blank,
                0
            );
            
         const auto url =
            Capture(
               Repeat(segment) and
               Optional(query),
               _url
            );
            
         const auto parser = 
            method and blanks and
               url and blanks and
               version and newLine and
            Headers(
               [this](Header* header) {
                  _headers.emplace(
                     header->_key,
                     header->_value
                  );
               }
            ) and
            Invoke(
               newLine,
               [this](Parser*) {
                  if (_method == "GET")
                     setResult(true);
                  return true;
               }
            ) and
            LoadOnDemand(loadBody, this);

         return parser;
      }
   
   
      void close() {
         if(_socket > 0)
            ::close(_socket);
         _socket = -1;
      }

      friend ostream& operator << 
      (ostream& out, const WebRequest& request)
      {
         out <<
            request._ipAddress << " " <<
            request._method << " " <<
            request._url << " " <<
            request._version << "\r\n";

         for (auto pair : request._headers)
         {
            out
               << pair.first
               << ": "
               << pair.second
               << "\r\n";
         }

         out << "\r\n";

         return out;
      }

      virtual bool read()
      {
         ssize_t size;
         size_t bufferSize = getPageSize();
         char* buffer = new char[bufferSize];
         bool success = true;
         _result = nullopt;

         while ( _result == nullopt )
         {
            size = ::read(
                _socket,
                buffer,
                bufferSize);
            
            if (size < 0)
            {
               stringstream stream;
               stream << "Error reading from " << _ipAddress;
               logMessage(LOG_NOTICE, stream.str());
               throw runtime_error("Error reading from web request");
            }
            
            if (size == 0) {
               usleep(100);
               continue;
            }

            if (!read(buffer, size))
            {
               success = false;
               break;
            }

         }

         delete[] buffer;

         success = success && flush();

         if (!success)
            setResult(false);

         return success;

      }

      virtual bool flush()
      override
      {
         if (!Parser::flush())
            return false;

         if (_body && !_body->flush())
            return false;

         return _parser.flush();
      }

      virtual Size write(
         const char* data,
         size_t size
      )
      {
         ssize_t written = 
            ::write(
               _socket,
               data,
               size
            );

         if (written != size)
         {
            stringstream stream;
            stream << "Error writing to " << _ipAddress;
            logMessage(LOG_NOTICE, stream.str());
            throw runtime_error("Error writing to web request");
         }
         
         return (Size)written;
      }

      virtual Size write(
         const std::string& data
      )
      {
         return write(data.data(), data.size());
      }

      bool pollInput() {
         struct pollfd pfds[1];
         pfds[0].fd = _socket;
         pfds[0].events = POLLIN;
         return (
            poll(
                 pfds, 1,
                 READ_TIMEOUT_SECONDS * 1000
              ) > 0 &&
            (pfds[0].revents & POLLIN) > 0
         );
      }

      virtual string getErrorMessage() const 
      override
      {
         stringstream stream;

         if (_body) {
            stream << "Invalid content from position "
                   << _body->_byteCount;
         }
         else
            stream << "Invalid content";

         return stream.str();
      }

      virtual bool read(char c)
      override
      {
         if (_result != nullopt)
            return false;

         Parser::read(c);

         bool matched = _parser.read(c);

         if (_parser._result != nullopt)
            setResult(_parser._result);
         else if (_contentLength > 0) {
            if (++_bytesRead == _contentLength)
            {
               if (_body)
                  _body->setResult(true);
               _parser.setResult(true);
               setResult(true);
            }
         }

         return matched;
      }

      virtual void success()
      override
      {
         Parser::success();
      }

      virtual void fail()
      override
      {
         Parser::fail();
      }

      
      static Parser* loadBody(void* params) {
         
         if (!params)
            throw logic_error("WebRequest::loadBody invalid params");

         WebRequest* request =
            (WebRequest*)(params);

         Parser* body = request->createBody();

         return body;
      }

      virtual Parser* createBody() {
         
         if (_headers.count("content-length") > 0)
         {
            string contentLength =
               _headers["content-length"];
                  
            _contentLength =
               atol(contentLength.c_str());
            _bytesRead = 0;
         }

         if ( _method == "POST" )
         {
            string contentType =
               _headers["content-type"];

            const string prefix = "application/json";

            if ( contentType
                    .find(prefix) !=
                    std::string::npos )
            {
               _body = createJSONBody();
               return _body;
            }
            else if (_contentLength > 0)
            {
               _body = createContentLengthBody();
               return _body;
            }


         }


         //throw logic_error("Shouldnt reach here");

         _body = new Optional(newLine);
         return _body;
      }

      virtual Parser* createJSONBody()
      {
         return new JSON();
      };

      virtual Parser*
      createContentLengthBody(
      )
      {
         return new
            ContentLength(
               _contentLength
            );
      }
      
      class ContentLength :
         public Parser
      {
      protected:
         size_t _contentLength;
         size_t _count {0};
      public:
         ContentLength(size_t contentLength) :
            _contentLength(contentLength)
         {
         }

         virtual bool read(char c)
         override
         {
            if (_count >= _contentLength)
               return false;

            ++_count;

            if (_count == _contentLength)
               setResult(true);

            return true;
         }

         virtual Parser* copy() const
         override
         {
            return new ContentLength(_contentLength);
         }
      };


   };

   
}

#endif