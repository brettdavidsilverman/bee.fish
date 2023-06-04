#include <iostream>
#include "../Test/Test.hpp"
#include "PowerEncoding.hpp"
#include "Encoding.hpp"


using namespace std;
using namespace BeeFishTest;
using namespace BeeFishMisc;

namespace BeeFishPowerEncoding
{

   inline bool test() {
   
      bool success = true;
      
      cout << "Test PowerEncoding" << endl;

      EncodeToStream encoder(cin, cout);
      encoder << 1;
      cout << endl;

      encoder << "Hello World";
      cout << endl;

      outputSuccess(success);

      return success;
   
   }

}