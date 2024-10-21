#include <stdlib.h> 
#include <stdio.h> 
#include <iostream>
#include <thread>
#include <utility>
#include "https.h"

#ifdef SERVER

#include <linux/limits.h>

#include "test.h"

using namespace BeeFishDatabase;
using namespace BeeFishHTTPS;

void start(
   boost::asio::io_context& io_context,
   const BString& host,
   int port,
   const BString& databaseFile,
   const BString& transactionFile
);

std::mutex _wait;

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

      
      unsigned int port;
      int portArg;
      if ((portArg = hasArg(argc, argv, "-port")) >= 0)
      {
         port = std::atoi(argv[portArg + 1]);
      }
      else
         port = 443;
         
      std::cout << "Host: " << HOST << ":" << port << endl;
      
      initializeLogs();
      
      std::cout << "Setting up App Factories" << std::endl;

      appFactories.add<AuthenticationApp>();
      //appFactories.add<StorageApp>();
      appFactories.add<FileSystemApp>();
      appFactories.add<JSONApp>();
      appFactories.add<NotFoundApp>();
      boost::asio::io_context io_context;
      _wait.lock();
      
      std::thread startThread(
         start, 
         std::ref(io_context),
         HOST,
         port,
         databaseFile,
         transactionFile
      );
      
      _wait.lock();
      
      bool test = false;
      if (hasArg(argc, argv, "-test") >= 0)
      {
          test = true;
      }
      
      if (test) {
         if (!BeeFishHTTPS::test())
            io_context.stop();
      }
      
      startThread.join();
      
   }
   catch (std::exception& e)
   {
      std::cerr << "Exception: " << e.what() << std::endl;
      return -1;
   }
   catch(...) {
      std::cerr << "Unkown exception" << std::endl;
   }

   cerr << "main shoudln't quit" << endl;
   
   return 0;
}

void start(
   boost::asio::io_context& io_context,
   const BString& host,
   int port,
   const BString& databaseFile,
   const BString& transactionFile
)
{
   std::cout << "Starting on port " << port << "..." << endl;

   BeeFishHTTPS::Server
      server
      (
         host,
         databaseFile,
         transactionFile,
         io_context,
         port
      );

   std::cout << "Started " << server.hostName() << " 😊" << endl;
      
   _wait.unlock();
   
   io_context.run();
}

#else
int main(int argc, const char* argv[])
{
   std::cerr << "Https only runs on server" << std::endl;
   return 0;
}
#endif

