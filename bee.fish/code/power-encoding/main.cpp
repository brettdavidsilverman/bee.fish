#include <fstream>
#include <array>
#include "../misc/misc.h"
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
   
   EncodeToStream encoding(cin, cout);
   
   while (!cin.eof())
   {
   
      BString line;
      
      getline(cin, line);
      
      if (line.size() == 0)
         break;
      
      Data data = Data::fromBase64(line);
      BitStream stream = BitStream::fromData(data);
      
      BString string;
      stream >> string;
      
      cout << string
           << endl;
      
   }
   
   return 0;
}
