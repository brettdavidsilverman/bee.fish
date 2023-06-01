#ifndef BEE_FISH__WEB__WEB_REQUEST_H
#define BEE_FISH__WEB__WEB_REQUEST_H

#include <map>
#include <vector>
#include <typeinfo>
#include <algorithm>
#include <cctype>
#include <string>
#include <poll.h>

#include "../Parser/Test.hpp"
#include "../JSON/Test.hpp"

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
            Repeat(not blankSpace),
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
      public map<std::string, std::string>,
      public Repeat
   {
   public:
      Headers() : Repeat(Header())
      {
      }

      Headers(const Headers& source)
      :
         map<std::string, std::string>(source),
         Repeat(Header())
      {
      }

      virtual void matchedItem(
         Parser* item
      )
      override
      {
         Header* header =
            dynamic_cast<Header*>(item);

         if (header)
            emplace(header->_key, header->_value);
      }

      virtual Parser* copy() const
      override
      {
         return new Headers(*this);
      }

      const string& operator[] (const string& key) const
      {
         return at(key);
      }

   };

   const Headers headers;

   auto _WebRequest =
   []( string& method,
       string& url,
       string& version )
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
         headers and
         newLine;

   };

 
   class WebRequest : public And {
   protected:
      int _socket;
   public:
      string _ipAddress;
      string _method;
      string _url;
      string _version;
      const int _headersIndex = 6;

   public:
      using Parser::read;

      WebRequest() :
         And(
            _WebRequest(
               _method,
               _url,
               _version
            )
         ),
         _socket(-1)
      {
      }

      WebRequest(int socket, const std::string& ipAddress)
      :  And(
            _WebRequest(
               _method,
               _url,
               _version
            )
         ),
         _socket(socket),
         _ipAddress(ipAddress)
      {
      }

      WebRequest(const WebRequest& source)
      : And(
            _WebRequest(
               _method,
               _url,
               _version
            )
         ),
         _socket(-1),
         _ipAddress(source._ipAddress)
      {
      }

      virtual ~WebRequest() {
         if(_socket > 0)
            ::close(_socket);
      }

      friend ostream& operator << 
      (ostream& out, const WebRequest& request)
      {
         out <<
            request._ipAddress << " " <<
            request._method << " " <<
            request._url << " " <<
            request._version << "\r\n";

         const Headers& headers = request.headers();

         for (auto pair : headers)
         {
            out
               << pair.first
               << ":"
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
            delete this;
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

        
         delete this;

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
               cerr.write((char*)buff, ret);

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

      const Headers& headers() const {
         Parser* parser = _inputs[_headersIndex];

         Headers* value =
            dynamic_cast<Headers*>(parser);

         if (value == nullptr)
            throw logic_error("Invalid headers index");

         return *value;

      }

   };
}

#endif