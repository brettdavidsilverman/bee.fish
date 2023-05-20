#ifndef BEE_FISH__WEB_SERVER__HPP
#define BEE_FISH__WEB_SERVER__HPP

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

#include "config.hpp"

namespace BeeFish {


   class WebRequest;

   class WebServer {
   protected:
      const int m_port;
      int m_serverSocket = -1;
      boost::asio::thread_pool m_threadPool;
      std::thread* m_loopThread = nullptr;
      bool m_stop = false;
      std::string m_host;

   public:
      WebServer(int port = WEB_SERVER_PORT, int threads = WEB_SERVER_THREADS) :
         m_port(port),
         m_threadPool(threads)
      {
         std::stringstream stream;
         stream << WEB_SERVER_HOST << ":" << m_port << "/";
         m_host = stream.str();
         std::cerr << m_host << std::endl;
      }

      ~WebServer() {
         stop();
      }
      
      virtual void start() {
         using namespace std;

         cout << "Starting WebServer " << m_host << endl;
         
         if (!initializeServerSocket()) {
            throw runtime_error("initializeServerSocket failed");
         }

         m_stop = false;
         m_loopThread = new std::thread(WebServer::loop, this); 

      }

      virtual void stop() {
         using namespace std;

         cout << "Stopping WebServer" << endl;

         std::stringstream stream;
         stream << "./stop.sh " << m_port;
         std::string command = stream.str();
         system(command.c_str());
     
         throw runtime_error("Should not reach here");
         
      }

      virtual void join() {
         if (m_loopThread) {
            m_loopThread->join();
         }
         m_threadPool.join();
      }

      virtual std::string host() {
         return m_host;
      }

      static void loop(WebServer* webServer) {

         using namespace std;

         cout << "WebServer loop started" << endl;

         while (!webServer->m_stop) {

            socklen_t clilen;
            struct sockaddr_in cli_addr;
            int clientSocket = -1;

            // Process will wait here till connection is accepted
            clilen = sizeof(cli_addr);

            cerr << "accept" << endl;

            clientSocket = accept(
               webServer->m_serverSocket,
               (struct sockaddr *)&cli_addr,
               &clilen
            );

            cerr << "accepted" << endl;
               
            if (clientSocket >= 0 &&
                !webServer->m_stop)
            {
               // Set client socket to non blocking
               fcntl(clientSocket, F_SETFL, O_NONBLOCK);
               webServer->handleRequest(clientSocket);
            }

         }
         
         webServer->close();
 
         cout << "WebServer loop ended" << endl;

      }

      bool initializeServerSocket() {
         
         using namespace std;

         //char buffer[128];
         struct sockaddr_in serv_addr;
         int opt = 1;

         if (m_serverSocket >= 0) {
            ::close(m_serverSocket);    
         }

         // First call to socket() function
         m_serverSocket = socket(AF_INET, SOCK_STREAM, 0);

         if (m_serverSocket < 0)
         {
            cerr << "Error creating server socket" << endl;
            return false;
         }

         // Set socket options
         if ( setsockopt(
                 m_serverSocket,
                 SOL_SOCKET,
                 SO_REUSEADDR |
                    SO_REUSEPORT, &opt,
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
         serv_addr.sin_port = htons(m_port);
         
         // Now bind the host address using bind() call.
         if (bind(m_serverSocket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
         {
            cerr << "Error binding server socket" << endl;
            return false;
         }

         // Now start listening for the clients,
         int res = listen(m_serverSocket, WEB_SERVER_THREADS);

         if (res != 0) {
            cerr << "Invalid listen result" << endl;
            return false;
         }

         cout << "Server listening on port " << m_port << endl;

         return true;

      }

      virtual void handleRequest(int clientSocket) {
  
         std::cerr << "handleRequest(" << clientSocket << ")" << std::endl;

         boost::asio::post(
            m_threadPool,
            [this, clientSocket]() {
               std::stringstream readInput;
               while (pollInput(clientSocket))
               {
                  int ret;
                  char buff[512];
                  

                  while ((ret = ::read(
                          clientSocket,
                          buff,
                          sizeof(buff))
                       ) > 0)
                  {
                    //std::cerr.write(buff, ret);
                     readInput.write(buff, ret);
                     if (ret < sizeof(buff))
                        break;
                  }
                  if (ret < sizeof(buff))
                     break;
               }
         
               const std::string
                  output = readInput.str();
               
               std::stringstream writeOutput;
               writeOutput <<
                  "HTTP/2.0 200 OK\r\n" <<
                  "Content-Type: text/plain\r\n" <<
                  "Connection: keep-alive\r\n" <<
                  "Content-Length: " << output.length() << "\r\n" <<
                  "\r\n" <<
                  output;

               std::string response = writeOutput.str();
               //std::cerr << response << std::endl;
               ::write(
                  clientSocket,
                  response.c_str(),
                  response.length()
               );
               ::close(clientSocket);
               return;
            }
         );

      }

      bool pollInput(int socket) {
         struct pollfd pfds[1];
         pfds[0].fd = socket;
         pfds[0].events = POLLIN;
         return (
            poll(
                 pfds, 1,
                 READ_TIMEOUT_SECONDS * 1000
              ) > 0 &&
            (pfds[0].revents & POLLIN) > 0
         );
      }

      virtual void close() {
         if (m_serverSocket > -1)
            ::close(m_serverSocket);
         m_serverSocket = -1;
      }

      static void sleep() {
         usleep(2L * 1000L * 1000L);
      }

   
   };

}


#endif
