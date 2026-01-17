#include <stdlib.h> 
#include <stdio.h> 
#include <iostream>
#include <thread>
#include <utility>
#include "https.h"

#ifdef SERVER

#include <linux/limits.h>

#include "https.h"
#include "test.h"

using namespace BeeFishDatabase;
using namespace BeeFishHTTPS;

void start(
   boost::asio::io_context& io_context,
   Server& server
);

std::mutex _wait;
const int _threadCount = 10;

int main(int argc, const char* argv[])

{
  
   try
   {
      BString databaseFile    = BEE_FISH_DATABASE_FILE;
      BString transactionFile = BEE_FISH_TRANSACTION_FILE;
      BString logFile         = BEE_FISH_SERVER_LOG_FILE;
      
      std::cout << "HTTPS Secure Server" << std::endl;
      std::cout 
        << "C++ run time: "
           << __cplusplus
           << std::endl
        << "Version: "
           << BEE_FISH_HTTPS_VERSION
           << std::endl
        << "Database file: "
           << databaseFile
           << std::endl
        << "Transaction file: "
           << transactionFile
           << std::endl
        << "Log file: "
           << logFile
           << std::endl;

      
      unsigned int port = 443;
      int portArg;
      if ((portArg = hasArg(argc, argv, "-port")) >= 0)
      {
          if (argc > portArg + 1)
          {
             const char* arg = argv[portArg + 1];
             char* p;
             long converted = strtol(arg, &p, 10);
             if (p)
                port = converted;
          }
      }
     
      initializeLogs();
      
      std::cout << "Setting up App Factories" << std::endl;

      appFactories.add<AuthenticationApp>();
      appFactories.add<FileSystemApp>();
      appFactories.add<StorageApp>();
      appFactories.add<JSONApp>();
      appFactories.add<NotFoundApp>();
      boost::asio::io_context io_context(_threadCount);
      
      std::cout << "Starting on port " << port << "..." << endl;

  
      BeeFishHTTPS::Server
      server
      (
         HOST,
         databaseFile,
         transactionFile,
         io_context,
         port,
         _threadCount
      );
      
      _wait.lock();
      
      std::thread startThread(
         start, 
         std::ref(io_context),
         std::ref(server)
      );
      
      _wait.lock();
      
      bool test = false;
      if (hasArg(argc, argv, "-test") >= 0)
      {
          test = true;
      }
      
      
      if (test) {
         bool ok = BeeFishHTTPS::test(port);
         io_context.stop();
         startThread.join();
         
         if (ok)
            return 0;
            
         return 1;
      }
      
      
      std::cout << "Started " << server.origin() << " 😊" << endl;
      
      startThread.join();
      
      
      return 0;
      
   }
   catch (std::exception& e)
   {
      std::cout << "Exception: " << e.what() << std::endl;
      return -1;
   }
   catch(...) {
      std::cout << "Unkown exception" << std::endl;
   }

   std::cout << "main shoudln't quit" << endl;
   
   return 0;
}

void start(
   boost::asio::io_context& io_context,
   Server& server
)
{
      
   _wait.unlock();
   
   io_context.run();
   
   server.join();
}

#else
int main(int argc, const char* argv[])
{
   std::cout << "Https only runs on server" << std::endl;
   return 1;
}
#endif

