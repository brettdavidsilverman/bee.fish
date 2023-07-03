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

      stringstream stream;
      EncodeToStream encoder(stream, stream);

      const unsigned long number = 19750926;
      encoder << number;
      cout << endl;

      unsigned long value;
      encoder >> value;
      cout << "Value: " << value << endl;
      success &= (value == number);

      cout << "Count: " << encoder._count << endl;
      success &= (encoder._count == 0);

      outputSuccess(success);

      return success;
   
   }

}