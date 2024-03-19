#include <iostream>

#include "../config.h"

#include "test.h"
#include "version.h"
#include "test-suite.h"

int hasArg(
   int argc,
   const char* argv[],
   const BString arg
);


int main(int argc, const char* argv[])
{

   std::cout << "Bee.Fish Test Suite" << std::endl;
   std::cout 
      << "C++ run time: "
         << __cplusplus
         << std::endl
      << "Version: "
         << BEE_FISH_TEST_VERSION
         << std::endl;
 
   cout << "Testing..." << endl;
     
   bool ok = BeeFishTest::test();
      
   if (ok)
      cout << "ALL Tests PASSED!" << endl;
   else
      cout << "ERROR!" << endl;

   return 0;
}

int hasArg(
   int argc,
   const char* argv[],
   const BString arg
)
{
   for (int i = 0; i < argc; i++)
   {
      if (arg == argv[i])
         return i;
   }
   
   return -1;
}