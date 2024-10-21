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
      
      Id id = Id::fromKey("/jHiY+MeJxPjHicR+J4Y+J4T4nJw+JyY+JyPicY+JxPiPhycY+HJxPhyY+HI+HE+GT4TycZPJw8Tk4mA");

      ok &= testResult(
         "Id from key",
         (id._name == "bee")
      );
      
      cout << endl;
      
      return ok;
   }
   
   inline bool testIds()
   {
      cout << "Testing large array of ids " << flush;
      ofstream ofile("test.txt");
      vector<Id> array(1000);
      map<std::string, int> map;
      
      for (Id& id : array)
      {
         //cout << id.key() << endl;
         ofile << id.key() << endl;
      }

      ofile << endl;

      ofile.close();


      ifstream ifile("test.txt");
      string key;
      bool duplicates = false;
      
      for (;;) {
         getline(ifile, key);
         if (key == "")
            break;
         if (map.count(key) == 0)
            map[key] = 1;
         else {
            map[key]++;
            cout << "Duplicate key" << endl;
            duplicates = true;
         }
         
         Id id = Id::fromKey(key);
         //cout << id.toString() << endl;
      }

      ifile.close();
      
      remove("test.txt");
      
      bool ok = !duplicates;
      
      BeeFishMisc::outputSuccess(ok);
      
      return ok;
   
   }
   

}

#endif
