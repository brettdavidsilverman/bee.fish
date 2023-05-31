#ifndef BEE_FISH__WEB__WEB_REQUEST_H
#define BEE_FISH__WEB__WEB_REQUEST_H

#include <map>
#include <vector>
#include <typeinfo>
#include <poll.h>

#include "../Parser/Parser.hpp"
#include "../JSON/JSON.hpp"

#include "Config.hpp"

namespace BeeFishWeb {

   using namespace BeeFishParser;
   using namespace BeeFishJSON;

   class WebRequest : public Parser {
   protected:
      int _socket;
      std::string _ipAddress;
      std::string _method;
      std::string _url;
      std::string _version;

      And _request =
         Capture(
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
         Word("HTTP/") and
         Capture(
            Repeat(Range("0", "9")) and
            Character(".") and
            Repeat(Range("0", "9")),
            _version
         ) and newLine;

   public:
      using Parser::read;

      WebRequest(int socket, const std::string& ipAddress)
      : Parser(),
         _socket(socket),
         _ipAddress(ipAddress)
      {
      }

      void process() {
         read();

         std::string output = 
            _ipAddress + " " +
            _method + " " +
            _url + " " +
            _version + "\r\n";

         std::stringstream writeOutput;

         writeOutput <<
            "HTTP/2.0 200 OK\r\n" <<
            "Content-Type: text/plain\r\n" <<
            "Connection: keep-alive\r\n" <<
            "Content-Length: " <<
               output.length() << "\r\n" <<
            "\r\n" <<
            output;

         std::string response =
            writeOutput.str();

         ::write(
            _socket,
            response.c_str(),
               response.length()
            );

         ::close(_socket);

         delete this;

      }

      virtual void read()
      {
         while (_result == std::nullopt &&
                pollInput())
         {
            int ret;
            char buff[512];

            while (_result == nullopt &&
                     (ret = ::read(
                        _socket,
                        buff,
                        sizeof(buff))
                     ) > 0)
            {
               read((const char*)buff, ret);

               if (ret < sizeof(buff))
                   break;
            }
            if (ret < sizeof(buff))
               break;
         }

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


      virtual bool read(char character)
      override
      {
         bool matched =
            _request.read(character);

         if (_request._result == true)
            setResult(true);
         else if (_request._result == false)
            setResult(false);

         return matched;
      }

      virtual Parser* copy() const
      override
      {
         return new WebRequest(_socket, _ipAddress);
      }
   };

}

#endif