#include <iostream>
#include "Test.hpp"


int main(int argc, const char* argv[]) {

   using namespace std;
   using namespace BeeFishMisc;
   using namespace BeeFishWeb;
   using namespace BeeFishWebDB;
   using namespace BeeFishParser;

   cout << "bee.fish.db-server"
           << endl
        << "C++ run time: "
           << __cplusplus
           << endl
        << "Database version: "
           << DATABASE_VERSION
           << endl
        << "Web Server version: "
           << WEB_SERVER_VERSION
           << endl
        << "Version: "
           << DB_SERVER_VERSION
           << endl;

#ifdef DEBUG
   cout << "Debug mode" << endl;
#else
   cout << "Release mode" << endl;
#endif

   if (hasArg(argc, argv, "-help") >= 0) {
      cout
         << "   Usage: DBServer [arguments]" << endl
         << "   Arguments:" << endl
         << "      [-host host]" << endl
         << "      [-port port]" << endl
         << "      [-threads threads]" << endl
         << "      [-database database]" << endl
         << "      [-test]" << endl
         << "   Example:" << endl
         << "      ./DBServer -port 80 -threads 10 -database bee.fish.data" << endl;
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

   string filename = DATABASE_FILENAME;
   int filenameArgument = hasArg(argc, argv, "-database");

   if (filenameArgument >= 0 &&
       filenameArgument < argc)
   {
      filename = argv[filenameArgument + 1];
   }

   if (hasArg(argc, argv, "-test") >= 0) {
      if (!BeeFish::test()) {
         cerr << "BeeFish DBServer tests failed" << endl;
         return 1;
      }
      else
         cout << "All tests pass 😍" << endl;
      return 0;
   }

   openLog();

   DBServer dbServer(host, port, threads, filename);

   dbServer.start();

   syslog(LOG_NOTICE, "DBServer %s on port %i", host.c_str(), port);

   dbServer.join();

   closeLog();

   return 0;
}