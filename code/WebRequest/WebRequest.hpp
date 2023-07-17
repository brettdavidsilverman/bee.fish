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
#include "URL.hpp"
#include "Header.hpp"
#include "Config.hpp"

namespace BeeFishWeb {

   using namespace std;
   using namespace BeeFishMisc;
   using namespace BeeFishParser;
   using namespace BeeFishJSON;

   class WebServer;

   class WebRequest : public Parser {
   protected:
      WebServer* _webServer;

      int _socket;
   public:
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

   public:
      using Parser::read;

      WebRequest() :
         _webServer(nullptr),
         _socket(-1)
      {
      }

      WebRequest(WebServer* webServer, int socket, const std::string& ipAddress)
      :
         _webServer(webServer),
         _socket(socket),
         _ipAddress(ipAddress)
      {
      }

      WebRequest(const WebRequest& source)
      :
         _webServer(source._webServer),
         _socket(-1),
         _ipAddress(source._ipAddress)
      {
      }

      virtual ~WebRequest() {
         close();
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
         int size;
         size_t bufferSize = getpagesize();
         char* buffer = new char[bufferSize];
         bool success = true;
         _result = nullopt;

         while ( _result == nullopt )
         {
            size = ::read(
                _socket,
                buffer,
                bufferSize);
            
            if (size == 0) {
               usleep(1000);
               continue;
            }

            if (!read(buffer, size))
            {
               success = false;
               break;
            }

            if (size < bufferSize) {
//cerr << "Read less than buffer size: " << _result << endl;
               
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

      virtual bool write(
         const char* data,
         size_t size
      )
      {
         size_t written = 
            ::write(
               _socket,
               data,
               size
            );

         return (written == size);
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


      virtual Parser* copy() const
      override
      {
         return new WebRequest(*this);
      }

      virtual bool read(char c)
      override
      {
         if (_result != nullopt)
            return false;

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

      And createParser() {

         And parser = Capture(
            Word("GET") or
            Word("POST") or
            Word("DELETE"),
            _method
         ) and blanks and
         Invoke(
            Capture(
               Character("/") and
               Repeat(not blank, 0),
               _url
            ),
            [this](Parser* match) {
               if (_url[_url.size() - 1] 
                   != '/' &&
                   _url.find("?") == 
                   std::string::npos)
               {
                  _url += "/";
               }
               return true;
            }
         ) and blanks and
         Capture(
            Word("HTTP/") and
            integer and
            fraction,
            _version
         ) and newLine and
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

      static Parser* loadBody(Parser* params) {
         
         if (!params)
            throw logic_error("WebRequest::loadBody invalid params");

         WebRequest* request =
            dynamic_cast<WebRequest*>(params);

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