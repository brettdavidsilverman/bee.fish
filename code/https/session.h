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
#include "../Script/Script.hpp"
#include "server.h"
#include "response.h"





using namespace BeeFishWeb;
using namespace BeeFishScript;

namespace BeeFishHTTPS {

   class Response;
   class Server;
 
   class Session : public SSLSocket
   {
   protected:
      Server* _server;
      size_t _maxLength;
      BeeFishBString::Data _data =
         BeeFishBString::Data::create();
      WebRequest* _request;
      ScriptParser* _parser;
      Response* _response;
      string _tempFileName;
      std::fstream _tempFile;
      bool _isStillPosting = false;
      BString _exception;
      BString _ipAddress;
      // End Of File error value
      const int END_OF_FILE = 2;
   public:
      friend class Response;
      
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
         stream << "bee.fish." << (void*)this;
         _tempFileName =
            string(TEMP_DIRECTORY) +
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
         _parser = new ScriptParser(*_request);
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

      virtual void closeOrRestart()
      {
         
         if (_request->headers()["connection"] == "close")
         {
            // Close
            delete this;
            return;
         }
           
         // Restart
         start();

      }

      virtual void handleRead(
         const boost::system::error_code& error,
         size_t bytesTransferred
      )
      {
         
         if (error)
         {
            if (error.value() != END_OF_FILE)
            {
               logException("handleRead", error);
               delete this;
            }
            return;
         }
         
         if (bytesTransferred > 0)
         {
            if (_parser->result() == nullopt)
               _parser->read(_data, bytesTransferred);
         }
         else {
            if (_parser->result() == nullopt)
               _parser->eof();
         }
            
         if (_parser->result() == false)
         {
            logException("handleRead", BString("Parser match error: ") + _parser->getError());
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

         // Check if finished request
         if ( _parser->result() == true )
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
      

      void dumpTempFile()
      {

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
            clog << now()
               << ' '
               << ipAddress()          << ' '
               << _request->method()   << ' '
               << hostName() << _request->fullURL()  << ' '
               << std::endl;

            _response = new Response(
               this
            );
            
            _response->handleResponse();
            
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
            delete _response;
            _response = nullptr;
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

      
      virtual void logException(
         const BString& where,
         const BString& what
      )
      {
         BeeFishScript::Object error = {
            {
               "exception", BeeFishScript::Object {
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
      
      const BString& hostName() const
      {
         return server()->hostName();
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
            delete this;
         }
      }

      Server* server()
      {
         return _server;
      }
      
      const Server* server() const
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

      ScriptParser* parser() {
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
       /*
      {
         ifstream input(_session->tempFileName());
         int c;
         while ((c = input.get()) != -1)
         {
            cerr << (char)c;
         }
         cerr << endl;
      }
      */
      ifstream input(_session->tempFileName());
      
      parser.read(input);
      parser.eof();
      
      input.close();

      if (parser.result() == true) {
         return true;
      }
      else {
         return false;
      }

      throw std::logic_error("Should not reach here in session.h");      
   }

   WebRequest* App::request() {
      return _session->request();
   }
   
   // Declared in response.h
   const BString& Response::ipAddress() const
   {
      return _session->ipAddress();
   }
   
   // Declared in response-headers.h
   ResponseHeaders::ResponseHeaders(
      Session* session
   ) :
      ResponseHeadersBase(
         {
            { "access-control-allow-origin", session->hostName() }
         }
      )
   {
   }
   
   
   // Declared in response.h
   void Response::closeOrRestart()
   {
      _session->closeOrRestart();
   }
   

}

#endif