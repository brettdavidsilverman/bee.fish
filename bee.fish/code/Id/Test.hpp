#ifndef BEE_FISH_ID__TEST_HPP
#define BEE_FISH_ID__TEST_HPP

#include "Id.hpp"
#include "../test/test.h"

using namespace BeeFishTest;

namespace BeeFishId
{
   inline bool testId();
   inline bool testIds();
   
   inline bool test()
   {
   
      bool ok = true;
      
      ok = ok && testId();
      ok = ok && testIds();
      
      if (ok)
         cout << "SUCCESS" << endl;
      else
         cout << "FAIL" << endl;
         
      return ok;
   }
   
   inline bool testId()
   {
      cout << "Id" << endl;
      
      bool ok = true;
      
      Id id = Id::fromKey("/jHiY+MeJxPjHicR+J4cnD4nhyY+J4ck+J4cj4nhxk+J4cT4nhw+J4ZPieGPieE+J4PicnD4nJj4nJPicj4nGT4nE+Jw+Jj4j4cnGT4cnGPhycT4cnD4cmT4cmPhxk+HGPhxPhk+GPJxk8nGPJxPJk8mPJPIQA==");
      
      ///jHiY+MeJxPjHicR+J4cnD4nhyY+J4ck+J4cj4nhxk+J4cT4nhw+J4ZPieGPieE+J4PicnD4nJj4nJPicj4nE+Jw+Jk+JPiPhycY+HJxPhyc");

      ok &= testResult(
         "Id from key",
         (id._name == "bee")
      );
      
      cout << endl;
      
      return ok;
   }
   
   inline bool testIds()
   {
      ofstream ofile("test.txt");
      vector<Id> array(1000);
      for (Id& id : array)
      {
         cout << id.key() << endl;
         ofile << id.key() << endl;
      }

      ofile << endl;

      ofile.close();


      ifstream ifile("test.txt");
      string key;
      for (;;) {
         getline(ifile, key);
         if (key == "")
            break;
         Id id = Id::fromKey(key);
         cout << id.toString() << endl;
      }

      ifile.close();
      
      remove("test.txt");
      
      return true;
   
   }
   

}

#endif
