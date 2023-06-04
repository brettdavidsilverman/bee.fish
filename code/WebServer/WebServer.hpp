#ifndef BEE_FISH__WEB__WEB_SERVER__HPP
#define BEE_FISH__WEB__WEB_SERVER__HPP

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <signal.h>
#include <iostream>
#include <thread>
#include <string>
#include <sstream>
#include <vector>
#include <boost/asio/thread_pool.hpp>
#include <boost/asio/post.hpp>

#include "Config.hpp"
#include "../Database/Database.hpp"
#include "../WebRequest/WebRequest.hpp"
#include "Version.hpp"

namespace BeeFishWeb {

   using namespace BeeFishDatabase;
   using namespace BeeFishPowerEncoding;

   typedef Path<PowerEncoding> Path;

   class WebServer {
   protected:
      const int _port;
      int _serverSocket = -1;
      boost::asio::thread_pool _threadPool;
      std::thread* _loopThread = nullptr;
      bool _stop = false;
      std::string _host;

   public:
      WebServer(
         string host = WEB_SERVER_HOST,
         int port = WEB_SERVER_PORT,
         int threads = WEB_SERVER_THREADS
      )
      :
         _host(host),
         _port(port),
         _threadPool(threads)
      {
      }

      virtual ~WebServer() {
         if (_loopThread)
            delete _loopThread;
      }
      
      virtual void start() {
         using namespace std;

         cout << "Starting WebServer " << _host << endl;
         
         if (!initializeServerSocket()) {
            throw runtime_error("initializeServerSocket failed");
         }

         _stop = false;
         _loopThread = new std::thread(WebServer::loop, this); 

      }

      virtual void stop() {
         using namespace std;

         if (_loopThread == nullptr)
            return;

         cout << "Stopping WebServer" << endl;
         _stop = true;

         // Flush a request through
         // the system using curl
         std::stringstream stream;
         stream << "curl " << url();
         std::string command = stream.str();

         system(command.c_str());

         _loopThread->join();

         delete _loopThread;
         _loopThread = nullptr;
         
         cout << "WebServer stopped 🛑" << endl;
      }

      virtual void join() {
         if (_loopThread) {
            _loopThread->join();
         }
         _threadPool.join();
      }

      virtual string host() const {
         return _host;
      }

      virtual int port() const {
         return _port;
      }

      virtual string url() const {
         stringstream stream;
         stream << "http://"
                <<  host()
                << ":"
                <<  port()
                << "/";

         return stream.str();
       
      }

      virtual string version() const
      {
         return WEB_SERVER;
      }

      static void loop(WebServer* webServer) {

         using namespace std;

         cout << "WebServer loop started 🟢" << endl;

         while (!webServer->_stop) {

            socklen_t clilen;
            struct sockaddr_in cli_addr;
            int clientSocket = -1;

            // Process will wait here till connection is accepted
            clilen = sizeof(cli_addr);

            clientSocket = accept(
               webServer->_serverSocket,
               (struct sockaddr *)&cli_addr,
               &clilen
            );
            
            if (clientSocket >= 0) //&&
               // !webServer->_stop)
            {
               const char *ipAddress = inet_ntoa(cli_addr.sin_addr);
               // Set client socket to non blocking
               fcntl(clientSocket, F_SETFL, O_NONBLOCK);

               // Delegate to thread
               boost::asio::post(
                  webServer->_threadPool,
                  [webServer, clientSocket, ipAddress]() {
                     handleWebRequest(
                        webServer,
                        clientSocket,
                        ipAddress
                     );
                  }
               );

            }

         }

         webServer->close();

        // delete webServer->_loopThread;
        // webServer->_loopThread = nullptr;

         cout << "WebServer loop ended" << endl;

      }

      bool initializeServerSocket() {
         
         using namespace std;

         //char buffer[128];
         struct sockaddr_in serv_addr;
         int opt = 1;

         if (_serverSocket >= 0) {
            ::close(_serverSocket);    
         }

         // First call to socket() function
         _serverSocket = socket(AF_INET, SOCK_STREAM, 0);

         if (_serverSocket < 0)
         {
            cerr << "Error creating server socket" << endl;
            return false;
         }

         // Set socket options
         if ( setsockopt(
                 _serverSocket,
                 SOL_SOCKET,
                 SO_REUSEADDR |
                    SO_REUSEPORT |
                    SO_KEEPALIVE, &opt,
                 sizeof(opt))
         )
         {
            cerr << "Error setting socket options" << endl;
            return false;
         }

         // Initialize socket structure
         bzero((char *)&serv_addr, sizeof(serv_addr));

         serv_addr.sin_family = AF_INET;
         serv_addr.sin_addr.s_addr = INADDR_ANY;
         serv_addr.sin_port = htons(_port);
         
         // Now bind the host address using bind() call.
         if (bind(_serverSocket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
         {
            cerr << "Error binding server socket" << endl;
            return false;
         }

         // Now start listening for the clients,
         int res = listen(_serverSocket, WEB_SERVER_THREADS);

         if (res != 0) {
            cerr << "Invalid listen result" << endl;
            return false;
         }

         cout << "Server listening on port " << _port << endl;

         return true;

      }

      static void handleWebRequest(
         WebServer* webServer,
         int clientSocket,
         std::string ipAddress
      )
      {
         webServer->handleWebRequest(
            clientSocket,
            ipAddress
         );
      }

      virtual void handleWebRequest(
         int clientSocket,
         string ipAddress
      )
      {
         stringstream outStream;

         outStream << version() << endl;

         string out = outStream.str();

         stringstream writeOutput;

         writeOutput <<
            "HTTP/2.0 500 Error\r\n" <<
            "Content-Type: text/plain; charset=utf-8\r\n" <<
            "Connection: keep-alive\r\n" <<
            "Content-Length: " <<
               out.length() << "\r\n" <<
            "\r\n" <<
            out;

         std::string response =
            writeOutput.str();

         ::write(
            clientSocket,
            response.c_str(),
            response.length()
         );


      }
      
      virtual void close() {
         if (_serverSocket > -1)
            ::close(_serverSocket);
         _serverSocket = -1;
      }
   
   };

}


#endif
