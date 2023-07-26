#include <iostream>
#include "../Test/Test.hpp"
#include "PowerEncodingBase.hpp"
#include "Encoding.hpp"


using namespace std;
using namespace BeeFishTest;
using namespace BeeFishMisc;
using namespace BeeFishPowerEncoding;

namespace BeeFishPowerEncoding
{

   inline bool testEncodeDecodeCharacter();
   inline bool testEncodeDecodeString();
   inline bool testEncodeDecodeNumber();
   inline bool test() {
   
      bool success = true;
      
      cout << "Test PowerEncoding" << endl;

      success = success &&
         testEncodeDecodeCharacter();

     success = success &&
         testEncodeDecodeString();

      success = success &&
         testEncodeDecodeNumber();
      
      outputSuccess(success);

      return success;
   }

   inline bool testEncodeDecodeCharacter()
   {
      cout << "Encode/Decode Character " << endl;
      bool success = true;
      stringstream stream;
      EncodeToStream encoder(stream, stream);

      char b = 'B';
      cout << "\tCharacter: " << b << ":" <<  (int)b << endl;
      encoder << b;

      char _b;
      encoder >> _b;
      cout << "\tCharacter: " << _b << ":" << (int)_b << endl;

      cout << "\tCount: " << encoder.count() << endl;

      success &= (b == _b);
      success &= (encoder.count() == 0);

      outputSuccess(success);

      return success;
   }

   inline bool testEncodeDecodeString()
   {
      cout << "\tString " << flush;
      bool success = true;
      stringstream stream;
      EncodeToStream encoder(stream, stream);

      string str = "Hello World 🌎";

      encoder << str;
      string _str;
      encoder >> _str;

      long int readCount = encoder.count();

      cout << "\tStrings equal "
           << (str == _str)
           << endl
           << "\t" << _str
           << endl
           << "\tWrite Count: "
           << readCount
           << endl
           << "\tFinal Count: "
           << encoder.count()
           << endl;

      success &= (str == _str);
      success &= (encoder.count() == 0);

      outputSuccess(success);

      return success;
   }

   inline bool testEncodeDecodeNumber()
   {
      bool success = true;
      stringstream stream;
      EncodeToStream encoder(stream, stream);

      const unsigned long number = 19750926L;
      encoder << number;
      cout << endl;

      unsigned long value;
      encoder >> value;
      cout << "Value: " << value << endl;
      success &= (value == number);

      cout << "Count: " << encoder.count() << endl;
      success &= (encoder.count() == 0);

      outputSuccess(success);

      return success;
   
   }

}