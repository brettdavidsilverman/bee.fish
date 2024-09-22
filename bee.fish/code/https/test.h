#ifndef BEE_FISH_HTTPS__TEST_H
#define BEE_FISH_HTTPS__TEST_H
#include "../test/test.h"

#include "../json/json-parser.h"

using namespace std::filesystem;
using namespace BeeFishTest;

namespace BeeFishHTTPS
{

   inline bool test()
   {
   
      bool ok = true;

      if (ok)
         cout << "SUCCESS" << endl;
      else
         cout << "FAIL" << endl;
         
      return ok;
   }
   
      
}

#endif
