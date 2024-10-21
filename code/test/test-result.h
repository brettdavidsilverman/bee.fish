#ifndef BEE_FISH_TEST__TEST_RESULT_H
#define BEE_FISH_TEST__TEST_RESULT_H

namespace BeeFishTest
{
   inline bool testResult(
      string label,
      bool ok
   )
   {
      cout << '\t' << label << ":\t";
      BeeFishMisc::outputSuccess(ok);
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


