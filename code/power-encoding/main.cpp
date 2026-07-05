#include <fstream>
#include <array>
#include "../Miscellaneous/Miscellaneous.hpp"
#include "../b-string/b-string.h"
#include "version.h"
#include "power-encoding.h"
#include "test.h"

using namespace std;
using namespace BeeFishMisc;
using namespace BeeFishBString;
using namespace BeeFishPowerEncoding;

int main(int argc, const char* argv[])
{
   cout << "bee.fish.server.power-encoding "
           << endl
        << "C++ run time: "
           << __cplusplus
           << endl
        << "Version: "
           << BEE_FISH_SERVER_POWER_ENCODING_VERSION
           << endl;
   
   bool test =
      (hasArg(argc, argv, "-test") != -1);
      
   if (test) {
      if (BeeFishPowerEncoding::test())
         return 0;
      return 1;
   }
   
   stringstream inout;
   
   EncodeToStream encoding(inout, inout);
   
   while (!cin.eof())
   {
   
      BString line;
      
      getline(cin, line);
      
      if (line.size() == 0)
         break;
      
      encoding << line;
      
      BString line2;
      
      encoding >> line2;
      
      cout << line2 << endl;
      
   }
   
   return 0;
}
