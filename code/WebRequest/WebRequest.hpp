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
      Parser* _body = nullptr;
      string _capture;
      string _contentType;
      And* _parser = createParser();

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
         delete _parser;
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
         int ret;
         char buffer[512];
         char *buff = &(buffer[0]);
         _result = nullopt;

         while ( _result == nullopt &&
                 pollInput() &&
                 (ret = ::read(
                     _socket,
                     buff,
                     sizeof(buffer))
                  ) > 0)
         {
            if (!read(buff, ret))
            {
               return false;
            }

            if (ret < sizeof(buffer))
               break;
         }

         // Read an additional
         // "End of file" character
         return readEndOfFile();

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

         if (_parser->_result != nullopt)
            return false;

         bool matched = _parser->read(c);

         if (_parser->_result != nullopt)
            setResult(_parser->_result);

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

      And* createParser() {

         And parser = Capture(
            Word("GET") or
            Word("POST") or
            Word("DELETE"),
            _method
         ) and blanks and
         Capture(
            Character("/") and
            Repeat(not blank, 0),
            _url
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
         newLine and
         LoadOnDemand(loadBody, this);

         return (And*)(parser.copy());
      }

      static Parser* loadBody(Parser* params) {
         
         if (!params)
            throw logic_error("WebRequest::loadBody invalid params");

         WebRequest* request =
            dynamic_cast<WebRequest*>(params);

         if (!request)
            throw logic_error("WebRequest::loadBody invalid webRequest");

         if ( request->_method == "POST" &&
              request->_headers.count("content-type") > 0 )
         {
            request->_contentType =
               request->_headers["content-type"];

            const string prefix = "application/json";

            if (
               request->_contentType
                  .rfind(prefix, 0) == 0)
            {

               Capture* capture =
                  new Capture(
                     _JSON(request),
                     request->_capture
                  );
               request->_body = capture;
               return request->_body;
            }
         }
               
         if ( request->_method == "POST" &&
              request->_headers.count("content-length") > 0)
         {
            string _contentLength =
               request->_headers["content-length"];
                  
            size_t contentLength =
               atol(_contentLength.c_str());

            cerr << "@@@@@@@Not implemented" << contentLength << endl;
            //throw logic_error("Not implemented yet");
         }

         request->_body = new Optional(newLine);
         return request->_body;
      }

      


   };

   
}

#endif