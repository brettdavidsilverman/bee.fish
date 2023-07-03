#include <fstream>
#include <array>
#include "Version.hpp"
#include "PowerEncoding.hpp"
#include "Test.hpp"

using namespace std;
using namespace BeeFishMisc;
using namespace BeeFishPowerEncoding;

int main(int argc, const char* argv[])
{
   cerr << "bee.fish.server.power-encoding "
           << endl
        << "C++ run time: "
           << __cplusplus
           << endl
        << "Version: "
           << BEE_FISH_SERVER_POWER_ENCODING_VERSION
           << endl;

   bool test =
      (hasArg(argc, argv, "-test") != -1);
   
   if (test)
   {
      if (BeeFishPowerEncoding::test())
         return 0;
         
      return 1;
   }

   EncodeToStream encoding(cin, cout);
   
   while (!cin.eof())
   {
   
      string line;
      
      getline(cin, line);
      
      if (line.size() == 0)
         break;
  
      encoding << atoi(line.c_str());
      cout << endl;

      
   }
   
   return 0;
}
