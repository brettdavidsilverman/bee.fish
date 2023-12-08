#include <iostream>
#include "../Miscellaneous/Miscellaneous.hpp"
#include "../Parser/Test.hpp"
#include "Test.hpp"
#include "WebServer.hpp"
#include <boost/json/src.hpp>

int main(int argc, const char* argv[]) {
   using namespace std;
   using namespace BeeFishMisc;
   using namespace BeeFishWeb;
   using namespace BeeFishParser;

   cout << endl;
   cout << WEB_SERVER_VERSION << endl;

#ifdef DEBUG
   cout << "Debug mode" << endl;
#else
   cout << "Release mode" << endl;
#endif

   if (hasArg(argc, argv, "-help") >= 0) {
      cout
         << "   Usage: WebServer [arguments]" << endl
         << "   Arguments:" << endl
         << "      [-host host]" << endl
         << "      [-port port]" << endl
         << "      [-threads threads]" << endl
         << "      [-test]" << endl
         << "   Example:" << endl
         << "      ./WebServer -port 80 -threads 10" << endl;
      return 0;
   }
   string host = WEB_SERVER_HOST;
   int hostArgument = hasArg(argc, argv, "-host");

   if (hostArgument >= 0 &&
       hostArgument < argc)
   {
      host = argv[hostArgument + 1];
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
      threads = atoi(argv[threadsArgument + 1]);
   }

   if (hasArg(argc, argv, "-test") >= 0) {
      if (!testWebServer()) {
         throw runtime_error("WebServer tests failed");
      }
      return 0;
   }

   WebServer webServer(host, port, threads);

   webServer.start();
   webServer.join();

   return 0;
}