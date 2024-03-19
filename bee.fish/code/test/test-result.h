#ifndef BEE_FISH_TEST__TEST_RESULT
#define BEE_FISH_TEST__TEST_RESULT

namespace BeeFishTest
{
   inline bool testResult(
      string label,
      bool ok
   )
   {
      cout << label << ":\t";
      
      if (ok)
         cout << "ok";
      else
         cout << "FAIL";

      cout << endl;
      
      return ok;
   }
   /*
   inline int hasArg(
      int argc,
      const char* argv[],
      const string& arg
   )
   {
      for (int i = 0; i < argc; i++)
      {
         if (arg == argv[i])
            return i;
      }
   
      return -1;
   }
   */
}

#endif


