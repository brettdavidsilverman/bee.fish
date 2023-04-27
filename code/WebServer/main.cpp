#include <iostream>
#include "../Miscellaneous/Miscellaneous.hpp"
#include "WebServer.hpp"

int main(int argc, const char* argv[]) {

   using namespace std;
   using namespace BeeFish;

   cout << "WebServer " << WEB_SERVER << endl;

   if (hasArg(argc, argv, "-help") >= 0) {
      cout
         << "   Usage: WebServer [arguments]" << endl
         << "   Arguments:" << endl
         << "      [-port port]" << endl
         << "      [-threads threads]" << endl
         << "   Example:" << endl
         << "      ./WebServer -port 80 -threads 10" << endl;
      return 0;
   }
   
   int port = WEB_SERVER_PORT;

   int portArgument = hasArg(argc, argv, "-port");

   if (portArgument >= 0 &&
       portArgument < argc)
   {
      port = atoi(argv[portArgument + 1]);
   }

   int threads = WEB_SERVER_THREADS;

   int threadsArgument = hasArg(argc, argv, "-threads");

   if (threadsArgument >= 0 &&
       threadsArgument < argc)
   {
      threads = atoi(argv[threadsArgument]);
   }

   WebServer webServer(port, threads);

   webServer.start();

   return 0;
}