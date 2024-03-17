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
   
   inline bool testCount();
   
   inline bool testEncodeDecodeCharacter();
   inline bool testEncodeDecodeString();
   inline bool testEncodeDecodeNumber();
   
   inline bool test() {
   
      bool success = true;
      
      cout << "Test PowerEncoding" << endl;

      success = success &&
         testCount();
         
      success = success &&
         testEncodeDecodeCharacter();

      success = success &&
         testEncodeDecodeString();

      success = success &&
         testEncodeDecodeNumber();
      
      outputSuccess(success);

      return success;
   }

   inline bool testCount()
   {
      cout << "Test Count" << endl;
      
      bool success = true;
      
      stringstream stream;
      EncodeToStream encoder(stream, cout);
      
      for (int i = 0; i <= 10; ++i)
      {
          encoder << i;
          cout << endl;
      }
      
      success &= (encoder.count() == 0);

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
      cout << "Encode/Decode String " << endl;
      bool success = true;
      stringstream stream;
      EncodeToStream encoder(stream, stream);

      string str = "Hello World 🌎";

      encoder << str;
      string _str;
      encoder >> _str;

      success &= testValue(str, _str);
      
      cout << "\tCount: " << encoder.count() << endl;
      success &= (encoder.count() == 0);
      
      outputSuccess(success);

      return success;
   }

   inline bool testEncodeDecodeNumber()
   {
      cout << "Encode/Decode Number " << endl;
      bool success = true;
      stringstream stream;
      EncodeToStream encoder(stream, stream);

      const unsigned long number = 19750926L;
      encoder << number;

      unsigned long value;
      encoder >> value;
      cout << "\tValue: " << value;
      success &= (value == number);
      outputSuccess(success);
      
      cout << "\tCount: " << encoder.count();
      success &= (encoder.count() == 0);

      outputSuccess(success);

      return success;
   
   }

}