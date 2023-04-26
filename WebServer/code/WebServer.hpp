#ifndef WEB_SERVER_BASE
#define WEB_SERVER_BASE

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
#include <vector>
#include "config.hpp"

namespace BeeFish {


    class WebRequest;

    class WebServer {
    public:
        const int _port;
        int _serverSocket = -1;

        WebServer(int port = 80) : _port(port) {
        }

        ~WebServer() {
           close();
        }
        
        virtual void start(int core = 0) {

            WebServer::loop(this); 

        }

        static void loop(void* param) {

            using namespace std;

            WebServer* webServer = (WebServer*)param;

            for (;;) {

                socklen_t clilen;
                struct sockaddr_in cli_addr;
                int clientSocket = -1;

                
                if (webServer->_serverSocket == -1) {
                  if (!webServer->initializeServerSocket()) {
                      cerr << "Error initializing server socket" << endl;
                      webServer->sleep();
                      continue;
                   }
                }


                // Process will wait here till connection is accepted
                clilen = sizeof(cli_addr);
                clientSocket = accept(webServer->_serverSocket, (struct sockaddr *)&cli_addr, &clilen);
                    
                if (clientSocket < 0)
                {
                   webServer->close();
                   continue;
                }
                
                std::cerr << "Web server request socket accepted: " << clientSocket << std::endl;

                webServer->handleRequest(clientSocket);

            }

        }

        bool initializeServerSocket() {
            
            using namespace std;

            char buffer[256];
            struct sockaddr_in serv_addr;

            if (_serverSocket > 0) {
                cerr << "Closing server socket " << _serverSocket << endl;
                ::close(_serverSocket);     
            }

            // First call to socket() function
            _serverSocket = socket(AF_INET, SOCK_STREAM, 0);

            if (_serverSocket < 0)
            {
                cerr << "Error creating server socket" << endl;
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
            int res = listen(_serverSocket, MAX_CLIENT_REQUESTS);

            if (res != 0) {
                cerr << "Invalid listen result" << endl;
                return false;
            }

            cerr << "Server listening on port " << _port << endl;

            return true;

         }

         virtual void handleRequest(int clientSocket) {
            const char* response =
               "HTTP/1.1 200 OK\r\n" \
               "Content-Type: text/plain\r\n" \
               "Connection: Close\r\n" \
               "\r\n" \
               "Hello World";
            ::write(clientSocket, response, strlen(response));
            ::close(clientSocket);
         }

         virtual void close() {
            if (_serverSocket > -1)
               ::close(_serverSocket);
            _serverSocket = -1;
         }

         virtual void sleep() {
            usleep(2L * 1000L * 1000L);
         }
    };

}


#endif
