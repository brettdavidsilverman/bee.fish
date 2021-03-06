#ifndef BEE_FISH_SERVER__SERVER_H
#define BEE_FISH_SERVER__SERVER_H

//
// server.h
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
#include "../database/database.h"
#include "session.h"

using namespace bee::fish::database;

namespace bee::fish::server {

class Server
{
public:
   Server( const std::string& hostName,
           const std::string databaseFile,
           boost::asio::io_context&
              ioContext,
           unsigned short port );
           
   ~Server();
   
   static std::string password();
   
   const std::string& hostName() const;
   Database* database();
   
   void startAccept();

   void handleAccept(
      Session* newSession,
      const boost::system::error_code&
         error
   );

   std::ofstream& log();
   
private:
   std::string _hostName;
   boost::asio::io_context& _ioContext;
   boost::asio::ip::tcp::acceptor _acceptor;
   boost::asio::ssl::context _context;
   Database* _database;
   std::ofstream _log;
};

}

#endif