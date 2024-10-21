#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include "../Miscellaneous/Miscellaneous.hpp"
#include "config.h"
#include "b-string.h"
#include "test.h"
#include "version.h"

using namespace std;
using namespace BeeFishMisc;
using namespace BeeFishBString;

int main(int argc, const char* argv[]) {
   
   cout << "bee.fish.b-string"
           << endl
        << "C++ run time: "
           << __cplusplus
           << endl
        << "Version: "
           << BEE_FISH_B_STRING_VERSION
           << endl;

   bool test =
      (hasArg(argc, argv, "-test") != -1);
      
   if (test)
   {
      cout << "Testing B-Strings" << endl;
      if (BeeFishBString::test())
         return 0;
         
      return 1;
   }
 
   cout << "Enter string" << endl;

   while (!cin.eof())
   {
 
      BString line;
      getline(cin, line);
     
      if (line == "")
         break;
      
      Data data(line);
     
      cout << line << endl;
#ifdef SERVER
      cout << data.sha3() << endl;
#endif

   }
  
   cout << "Bye" << endl;
   
   return 0;
}
