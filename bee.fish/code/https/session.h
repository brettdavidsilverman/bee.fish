#ifndef BEE_FISH_HTTPS__SESSION_H
#define BEE_FISH_HTTPS__SESSION_H

//
// session.h
// ~~~~~~~~~~
//
// Modified from
// Copyright (c) 2003-2019 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <unistd.h>
#include <fstream>
#include "../web-request/web-request.h"
#include "../b-script/b-script.h"
#include "server.h"
#include "response.h"



typedef boost::asio::ssl::stream<boost::asio::ip::tcp::socket> SSLSocket;

using namespace BeeFishWeb;
using namespace BeeFishBScript;

namespace BeeFishHTTPS {

   class Response;
   class Server;
 
   class Session : public SSLSocket
   {
   protected:
      Server* _server;
      size_t _maxLength;
      Data _data = Data::create();
      WebRequest* _request;
      BScriptParser* _parser;
      Response* _response;
      string _tempFileName;
      std::fstream _tempFile;
      bool _isStillPosting = false;
      BString _exception;
      BString _ipAddress;
   public:

      Session(
         Server* server,
         boost::asio::io_context& ioContext,
         boost::asio::ssl::context& sslContext
      ) :
         SSLSocket(ioContext, sslContext),
         _server(server),
         _maxLength(getPageSize()),
         _request(nullptr),
         _parser(nullptr),
         _response(nullptr)
      {
         // Create the temp file name.
         stringstream stream;
         stream << this;
         _tempFileName =
            string(TEMP_DIRECTORY) +
            "/" +
            stream.str();
      }
  
  
      virtual ~Session()
      {
         clear();
         if (filesystem::exists(_tempFileName))
             remove(_tempFileName);
      }
      
      virtual void start()
      {
         clear();
         
         try
         {
            _ipAddress =  
               lowest_layer()
                  .remote_endpoint()
                  .address()
                  .to_string();
         }
         catch (...)
         {
            logException("Session::start", "Invalid ipAddress");
            delete this;
            return;
         }
         _request = new BeeFishWeb::WebRequest();
         _parser = new BScriptParser(*_request);
         asyncRead();
      }
   
      virtual void clear()
      {
         if (_request)
         {
            delete _request;
            _request = nullptr;
         }
         
         if (_parser)
         {
            delete _parser;
            _parser = nullptr;
         }
         
         if (_response)
         {
            delete _response;
            _response = nullptr;
         }
         
         if (_tempFile.is_open())
         {
            _tempFile.close();
         }
         
         _exception = "";
         
      }

      virtual bool closeOrRestart() {
         if (!_response || _response->end()) {
            if (_request->headers()["connection"] == "close") {
               // Close
               delete this;
               return true;
            }
            // Restart
            start();
            return true;
         }

         return false;

      }

      virtual void handleRead(
         const boost::system::error_code& error,
         size_t bytesTransferred
      )
      {
         if (error)
         {
            logException("handleRead", error);
            delete this;
            return;
         }
         
         if (bytesTransferred > 0)
         {
#ifdef DEBUG
//            cerr << "Bytes transfeferred: " << bytesTransferred << endl;            
//            cerr.write((const char*)_data._data, bytesTransferred);
#endif
            _parser->read(_data, bytesTransferred);

            if (_request->result() == false)
            {
               logException("handleRead", "parser match error");
#ifdef DEBUG
               dumpTempFile();
#endif
               delete this;
               return;
            }
            
               
            if ( _request->_headers &&
                 _request->headers().result() == true && 
                 _request->method() == "GET" )
            {
               handleResponse();
               return;
            }

            // Write current session data to file
            if (!_tempFile.is_open())
            {
               try
               {
                  openTempFile();
               }
               catch (...)
               {
                  delete this;
                  return;
               }
            
            }
            _tempFile.write((const char*)_data._data, bytesTransferred);
         }
         else {
            if (_request->result() != true)
            {
               logException("handleRead", "Parser input incomplete");
               delete this;
               return;
            }
         }

         // Check if finished request
         if ( _request->result() == true )
         {
            _tempFile.close();

            //dumpTempFile();

            _server->appendToLogFile(_tempFileName);

            handleResponse();
            return;
         }
         
         // More data to come...
         asyncRead();
      }
      

      void dumpTempFile() {

         ifstream input(_tempFileName);

         std::cout << "File Dump of " << _tempFileName << std::endl;

         int c;
         while (!input.eof()) {
            c = input.get();
            if (c == -1)
               break;
            std::cout << (char)c;
         }

         std::cout << std::endl;

         input.close();

      }

      void handleResponse() 
      {

         try 
         {

            // All input is now in
            Server::writeDateTime(clog);

            clog
               << '\t'
               << ipAddress()          << '\t'
               << _request->method()   << '\t'
               << HOST << _request->fullURL()  << '\t'
               << _request->version()
               << std::endl;

            _response = new Response(
               this
            );

            if (!closeOrRestart())
               asyncWrite();


         }
         catch (std::exception& ex) {
            logException("Session::handleResponse", ex.what());
            delete this;
            return;
         }
         catch (...)
         {
            logException("Session::handleResponse", "Unkown error");
            delete this;
            return;
         }
      }
      
      void openTempFile()
      {
         _tempFile.open(
            _tempFileName,
            std::fstream::in  |
            std::fstream::out |
            std::fstream::trunc
         );

         if (_tempFile.fail())
         {
            BString what = "Failed to open file: ";
            what += _tempFileName;
  
            logException(
               "Session::Start",
               what
            );
            
            throw runtime_error("Failed to open temp file");
         }
         
         permissions(
            _tempFileName,
               perms::group_read  |
               perms::group_write |
               perms::others_read |
               perms::others_write,
            perm_options::remove
         );
         
         permissions(
            _tempFileName,
            perms::owner_read |
               perms::owner_write,
            perm_options::replace
         );
 
      }
      
      
      void handleWrite(
         const boost::system::error_code&
            error
      )
      {
         if (error)
         {
            logException(
               "Session::handleWrite",
               error
            );
            delete this;
            return;
         }
   
         if (!closeOrRestart())
            asyncWrite();
      }
      
      virtual void asyncRead()
      {
         async_read_some(
            boost::asio::buffer(
               _data._readWrite,
               _maxLength
            ),
            boost::bind(
               &Session::handleRead,
               this,
               boost::asio::placeholders::error,
               boost::asio::placeholders::bytes_transferred
            )
         );

         
      }

      virtual void asyncWrite()
      {

         if (closeOrRestart())
            return;
            
         size_t length = _maxLength;
         
         Data data =
            _response->getNext(length);

         boost::asio::async_write(
            *this,
            boost::asio::buffer(
               data._data,
               length
            ),
            boost::bind(
               &Session::handleWrite,
               this,
               boost::asio::placeholders::error
            )
         );
      }

      virtual void logException(
         const BString& where,
         const BString& what
      )
      {
         BeeFishBScript::Object error = {
            {
               "exception", BeeFishBScript::Object {
                  {"where", where},
                  {"what", what},
                  {"ipAddress", ipAddress()},
                  {"who", getPointerString()},
                  {"when", Server::getDateTime()}
               }
            }
         };
         
         cerr << error << endl;
      }

      BString getPointerString() {
         stringstream stream;
         stream << this;
         return stream.str();
      }
      
      virtual void logException(
         const BString& where,
         const boost::system::error_code& error
      )
      {
      
         if (error.value() == 1)
            // session connection truncated
            return;

#ifdef DEBUG            
         if (error.value() == 336151574)
            // sslv3 alert certificate unknown
            return;
#endif            
            
         stringstream stream;
         stream << error.category().name() << ":" << error.value() << ":" << error.message();
   
         logException(where, stream.str());
      }
      
      virtual void logException(
         const BString& where
      )
      {
         stringstream stream;
         stream << strerror(errno);
   
         logException(where, stream.str());
      }
   
      SSLSocket::lowest_layer_type& socket()
      {
         return lowest_layer();
      }

      const BString& ipAddress() const
      {
         return _ipAddress;
      }
   
      void handshake()
      {
         SSLSocket::async_handshake(
            boost::asio::ssl::stream_base::server,
            boost::bind(
               &Session::handleHandshake,
               this,
               boost::asio::placeholders::error
            )
         );
      }


      void handleHandshake(const boost::system::error_code& error)
      {

         if (!error)
         {
            start();
         }
         else
         {
            /*
            logException(
               "Session::handleHandshake",
               error
            );
            */
            delete this;
         }
      }

      Server* server()
      {
         return _server;
      }
      
      WebRequest* request()
      {
         return _request;
      }

      void setWebRequest(WebRequest* request) {
         if (_request)
            delete _request;
         _request = request;
      }
      
      Response* response()
      {
         return _response;
      }

      BScriptParser* parser() {
         return _parser;
      }
  
      string tempFileName() {
         return this->_tempFileName;
      }
      
      const BString& exception() const
      {
         return _exception;
      }
   };
   
   // Declared in server.h
   inline void Server::startAccept()
   {
      
      Session* newSession =
         new Session(
            this, 
           _ioContext,
           _context
        );
   
      _acceptor.async_accept(
         newSession->socket(),
         boost::bind(
           &Server::handleAccept,
           this,
           newSession,
           boost::asio::placeholders::error
         )
      );
   }
   
   // Declared in server.h
   inline void Server::handleAccept(
      Session* newSession,
      const boost::system::error_code& error
   )
   {
      startAccept();

      if (!error)
      {
         newSession->handshake();
      }
      else
      {
         delete newSession;
      }

   }

   // Declared in authentication.h
   inline Authentication::Authentication(
      Session* session
   ) : Authentication(
          *( session->server()->database() ),
            session->ipAddress(),
            session->
               request()->
               getCookie("sessionId")
         )
   {
   }

   // Defined in app.h
   inline bool App:: parseWebRequest(
      Parser& parser
   )
   {
      
      ifstream input(_session->tempFileName());
      parser.read(input);

      input.close();

      if (parser.result() == true) {
         return true;
      }
      else {
         return false;
      }

      throw std::logic_error("Should not reach here in session.h");      
   }

   inline WebRequest* App::request() {
      return _session->request();
   }

}

#endif