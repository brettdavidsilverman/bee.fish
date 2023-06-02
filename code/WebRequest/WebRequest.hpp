#ifndef BEE_FISH__WEB__WEB_REQUEST_H
#define BEE_FISH__WEB__WEB_REQUEST_H

#include <map>
#include <vector>
#include <typeinfo>
#include <algorithm>
#include <cctype>
#include <string>
#include <poll.h>

#include "../Parser/Parser.hpp"
#include "../JSON/JSON.hpp"

#include "Config.hpp"

namespace BeeFishWeb {

   using namespace std;
   using namespace BeeFishParser;
   using namespace BeeFishJSON;

   class Headers;

   const auto colon = Character(":");
   auto _Header =
   [](string& key, string& value) {
      return
         Capture(
            Repeat(not (colon or blankSpace)),
            key
         ) and
         -blanks and colon and
         -blanks and
         Capture(
            Repeat(not (carriageReturn or lineFeed)),
            value
         ) and
         newLine;
   };

   class Header : public And {
   public:
      string _key;
      string _value;

   public:

      Header() : And(
        _Header(_key, _value)
      )
      {
      }

      Header(const Header& source) :
           _key(source._key),
           _value(source._value),
          And(_Header(_key, _value))
      {

      }

      virtual void success()
      override
      {

         // Convert key to lower case
         // Note that this only works
         // for latin characters
         std::transform(
            _key.begin(),
            _key.end(),
            _key.begin(),
            [](unsigned char c) {
               return std::tolower(c);
            }
         );

         And::success();

      }

      virtual Parser* copy() const
      override
      {
         return new Header(*this);
      }
   };

   class Headers :
      public Repeat
   {
   public:
      typedef std::function<void(Header*)> Function;
      Function _function;

      Headers(Function func = nullptr) :
         Repeat(Header()),
         _function(func)
      {
      }

      Headers(const Headers& source)
      :
         Repeat(Header()),
         _function(source._function)
      {
      }

      virtual void matchedItem(
         Parser* item
      )
      override
      {
         Header* header =
            dynamic_cast<Header*>(item);

         if (_function)
            _function(header);
      }

      virtual Parser* copy() const
      override
      {
         return new Headers(*this);
      }

   };

   auto _WebRequest =
   []( string& method,
       string& url,
       string& version,
       map<string, string>& headers )
   {
      return
         Capture(
            Word("GET") or
            Word("POST") or
            Word("DELETE"),
            method
         ) and blanks and
         Capture(
            Character("/") and
            Repeat(not blank, 0),
            url
         ) and blanks and
         Capture(
            Word("HTTP/") and
            integer and
            fraction,
            version
         ) and newLine and
         Headers(
            [&headers](Header* header) {
               headers.emplace(
                  header->_key,
                  header->_value
               );
            }
         ) and
         newLine;

   };

 
   class WebServer;

   class WebRequest : public And {
   protected:
      WebServer* _webServer;

      int _socket;
   public:
      string _ipAddress;
      string _method;
      string _url;
      string _version;
      map<string, string> _headers;

   public:
      using Parser::read;

      WebRequest() :
         And(
            _WebRequest(
               _method,
               _url,
               _version,
               _headers
            )
         ),
         _webServer(nullptr),
         _socket(-1)
      {
      }

      WebRequest(WebServer* webServer, int socket, const std::string& ipAddress)
      : And(
            _WebRequest(
               _method,
               _url,
               _version,
               _headers
            )
         ),
         _webServer(webServer),
         _socket(socket),
         _ipAddress(ipAddress)
      {
      }

      WebRequest(const WebRequest& source)
      :
         And(source),
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

      bool process() {
         using namespace std;

         if (!read()) {
cerr << "Read failure" << endl;
            close();
            return false;
         };

         std::stringstream output;

         output << *this;

         std::string out = output.str();

         std::stringstream writeOutput;

         writeOutput <<
            "HTTP/2.0 200 OK\r\n" <<
            "Content-Type: text/plain\r\n" <<
            "Connection: keep-alive\r\n" <<
            "Content-Length: " <<
               out.length() << "\r\n" <<
            "\r\n" <<
            out;

         std::string response =
            writeOutput.str();

         ::write(
            _socket,
            response.c_str(),
               response.length()
            );

        
         return true;

      }

      virtual bool read()
      {
         while (_result == std::nullopt &&
                pollInput())
         {
            int ret;
            char buffer[512];
            char *buff = &(buffer[0]);

            while (_result == nullopt &&
                     (ret = ::read(
                        _socket,
                        buff,
                        sizeof(buffer))
                     ) > 0)
            {
               if (!read((const char*)buff, ret))
               {
                  return false;
               }

               if (ret < sizeof(buffer))
                  return true;

            }

            if (ret < sizeof(buffer))
               return true;


         }

         return true;

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


   };

   
}

#endif