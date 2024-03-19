#include <iostream>
#include "../test/test.h"
#include "power-encoding-base.h"
#include "../b-string/string.h"
#include "../b-string/misc.h"
#include "encoding.h"


using namespace std;
using namespace BeeFishTest;
using namespace BeeFishBString;

namespace BeeFishPowerEncoding
{

   inline bool test() {
   
      bool ok = true;
      
      std::stringstream stream1;
      EncodeToStream encodingOut(stream1, stream1);

      for (int i = 0; i < 256; i++)
      {
         encodingOut << (unsigned char)i;
      }

      cout << "Count: " << encodingOut.count() << endl;
      ok &= testResult("Number count", encodingOut.count() == 0);
  
      stringstream strstream;
      EncodeToStream encoding(strstream, strstream);
      BString test;
      
      encoding << BString("Hello Bee");
      
      ok &= testResult(
         "Hello Bee write Count",
         ( encoding.count() == 0 )
      );

      encoding.reset();

      encoding >> test;

      cout << "Encoding count: " << encoding.count() << endl;

      ok &= testResult(
         "Hello Bee",
         ( test == "Hello Bee" )
      );

      ok &= testResult(
         "Hello Bee read Count",
         ( encoding.count() == 0 )
      );
    
    
      test.clear();
      
      encoding << BString("🍄");
      encoding >> test;
      ok &= testResult(
         "String 🍄",
         ( test == "🍄" ) &&
         ( encoding.count() == 0 )
      );

      encoding << 8;
      int i = 0;
      encoding >> i;
         
      ok &= testResult(
         "Number 8",
         ( i == 8 ) &&
         ( encoding.count() == 0 )
      );
      
      bool testBool;

      encoding << true;
      encoding >> testBool;
      ok &= testResult(
         "True",
         testBool == true
      );
      
      
      encoding << false;
      encoding >> testBool;
      ok &= testResult(
         "False",
         testBool == false
      );
  
      BString start = "ABC Hello World";
      BitStream stream;
      stream << start;
      stream.reset();
      BString finish;
      stream >> finish;
      
      ok &= testResult(
         "Bit Stream",
         start == finish
      );
      
      cout << finish << endl;
      cout << stream.toData().toBase64() << endl;
      
      if (ok)
         cout << endl << "SUCCESS" << endl;
      else
         cout << endl << "FAIL" << endl;
      
      return ok;
   
   }

}