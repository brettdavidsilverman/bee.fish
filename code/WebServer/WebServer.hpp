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
#include <chrono>
#include <boost/asio/thread_pool.hpp>
#include <boost/asio/post.hpp>

#include "../Miscellaneous/SigHandler.hpp"

#include "Config.hpp"
#include "../Database/Database.hpp"
#include "../WebRequest/WebRequest.hpp"
#include "Version.hpp"

namespace BeeFishWeb {

   using namespace std::chrono;

   using namespace BeeFishDatabase;
   using namespace BeeFishPowerEncoding;

   typedef Path<Database::Encoding> Path;

   class WebServer {
   protected:
      const int _port;
      int _serverSocket = -1;
      boost::asio::thread_pool _threadPool;
      std::thread* _loopThread = nullptr;
      bool _stop = false;
      bool _isRunning = false;
      std::mutex _running;
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
      
      virtual bool start() {
         using namespace std;

         logMessage(
            LOG_NOTICE,
            "Starting WebServer %s",
            _host.c_str()
         );
         
         if (!initializeServerSocket()) {
            return false;
         }

         _stop = false;
         _isRunning = false;

         _loopThread = new std::thread(WebServer::loop, this); 

         while (!_isRunning) {
            this_thread::sleep_for(milliseconds(100));
         }

         return true;

      }

      virtual void stop() {
         using namespace std;

         if (_loopThread == nullptr)
            return;

         logMessage(LOG_NOTICE, 
            "Stopping WebServer");

         // Flush a request through
         // the system using curl
         std::stringstream stream;
         stream << "curl " << url() << " -s > /dev/null &";
         std::string command = stream.str();
         
         _stop = true;

         while (_isRunning) {
            logMessage(LOG_NOTICE, "WebServer flush");
            system(command.c_str());
            this_thread::sleep_for(milliseconds(100));
         }

         logMessage(LOG_NOTICE, 
            "Waiting on lock guard");

         lock_guard<mutex>
            guard(_running);

         logMessage(LOG_NOTICE,
            "Waiting on loop thread");
         _loopThread->join();

         delete _loopThread;
         _loopThread = nullptr;

         logMessage(LOG_NOTICE,
            "Waiting on thread pool");

         _threadPool.join();

         close();

         logMessage(LOG_NOTICE,
            "WebServer stopped 🔴");
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
         return WEB_SERVER_VERSION;
      }

      static void loop(WebServer* webServer) {

         using namespace std;

         logMessage(LOG_NOTICE, "WebServer loop started  🟢");

         lock_guard<mutex> guard(webServer->_running);

         while (!webServer->_stop) {

            webServer->_isRunning = true;

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
            
            if (clientSocket >= 0 &&
                !webServer->_stop)
            {
               const char *ipAddress = inet_ntoa(cli_addr.sin_addr);

               //logMessage(LOG_NOTICE, ipAddress);
         
               // Set client socket to non blocking
               //fcntl(clientSocket, F_SETFL, O_NONBLOCK);

               // Delegate to thread
               boost::asio::post(
                  webServer->_threadPool,
                  [webServer, clientSocket, ipAddress]() {
                     webServer->handleWebRequest(
                        clientSocket,
                        ipAddress
                     );
                     return true;
                  }
               );

            }

         }

         webServer->_isRunning = false;

        // delete webServer->_loopThread;
        // webServer->_loopThread = nullptr;

         logMessage(LOG_NOTICE, "WebServer loop ended");
         
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
            perror("Error creating server socket");
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
            perror("Error setting socket options");
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
            perror("Bind server socket");
            return false;
         }

         // Now start listening for the clients,
         int res = listen(_serverSocket, WEB_SERVER_LISTEN_BUFFER);

         if (res != 0) {
            perror("Invalid listen result");
            return false;
         }

         cout << "Server listening on port " << _port << " 🟢" << endl;

         return true;

      }


      virtual bool handleWebRequest(
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

         return false;

      }
      
      virtual void close() {
         logMessage(LOG_NOTICE, "Closing web server");
         if (_serverSocket > -1)
            ::close(_serverSocket);
         _serverSocket = -1;
      }
   
   };

}


#endif
