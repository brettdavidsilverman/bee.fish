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
#include <fstream>
#include <chrono>
#include <ctime>
#include <unistd.h>
#include <mutex>
#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/asio/ssl/context.hpp>

#define SERVER

#include "../config.h"

#include "../database/database.h"
#include "../b-string/string.h"

#include "date.h"


using namespace BeeFishDatabase;


namespace BeeFishHTTPS {

   class Session;
   std::string my_password_callback(
      std::size_t max_length,  // the maximum length for a password
      boost::asio::ssl::context::password_purpose purpose
   );
   
   class Server
   {
   public:
      Server( const BString& hostName,
              const BString& databaseFile,
              const BString& transactionFile,
              boost::asio::io_context&
                 ioContext,
              unsigned short port
      ) :
         _hostName(hostName),
         _ioContext(ioContext),
         _acceptor(
            ioContext,
            /*
            boost::asio::ip::tcp::endpoint(
               boost::asio::ip::address::from_string("10.152.0.5"),
               port
            )
            */
            boost::asio::ip::tcp::endpoint(
               boost::asio::ip::tcp::v4(),
               port
            )
         ),
         _context(boost::asio::ssl::context::sslv23)
      {
         std::cout << "Opening transaction file " << std::endl;
   
         _transactionFile.open(
            transactionFile.str(),
            std::ofstream::out | std::ofstream::app
         );
   
         _context.set_options(
            boost::asio::ssl::context::default_workarounds
            | boost::asio::ssl::context::no_sslv2
            | boost::asio::ssl::context::single_dh_use
         );
         
         std::cout << "Setting up passwords..." << std::endl;
  
         _context.set_password_callback(
            my_password_callback
         );
   
         std::cout << "Setting up certificates..." << std::endl;

         _context.use_certificate_chain_file(CERT_FILE);
   
         _context.use_private_key_file(KEY_FILE, boost::asio::ssl::context::file_format::pem);
  
         _context.use_tmp_dh_file(TMP_DH_FILE);

         std::cout << "Setting up database..." << std::endl;

         _database = new Database(databaseFile);
   
         std::cout << "Start accepting..." << std::endl;

         startAccept();

         std::cout << "HTTPS server started" << std::endl;
      }

           
      ~Server()
      {
         _transactionFile.close();
      }
   
      static BString password()
      {
         return "test";
      }
   
      const BString& hostName() const
      {
         return _hostName;
      }
      
      Database* database()
      {
         return _database;
      }
      
      // Defined in session.h
      void startAccept();

      // Defined in session.h
      void handleAccept(
         Session* newSession,
         const boost::system::error_code&
            error
      );

      std::ofstream& log()
      {
         return _transactionFile;
      }


      void appendToLogFile(path inputFilePath) {
         ifstream input(inputFilePath);
         std::unique_lock<std::mutex> lock(_mutex);
         _transactionFile << input.rdbuf();
         _transactionFile << endl;
         input.close();
      }

      static void writeDateTime(ostream& out)
      {
         date::writeDateTime(out);
      }
      
      static BString getDateTime()
      {
         return date::getDateTime();
      }

   private:
      BString _hostName;
      boost::asio::io_context& _ioContext;
      boost::asio::ip::tcp::acceptor _acceptor;
      boost::asio::ssl::context _context;
      Database* _database;
      std::ofstream _transactionFile;
      std::mutex _mutex;
   };
   
   inline std::string my_password_callback(
      std::size_t max_length,  // the maximum length for a password
      boost::asio::ssl::context::password_purpose purpose ) // for_reading or for_writing
   {
      BString password = Server::password(); 
      // security warning: !! DO NOT hard-code the password here !!
      // read it from a SECURE location on your system
      return password;
   }




}

#endif