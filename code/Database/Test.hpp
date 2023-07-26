#ifndef BEE_FISH__DATABASE__TEST_HPP
#define BEE_FISH__DATABASE__TEST_HPP
#include "../Miscellaneous/Miscellaneous.hpp"

#include "../JSON/JSON.hpp"
#include "../Parser/Test.hpp"
#include "Database.hpp"


namespace BeeFishDatabase
{
   using namespace BeeFishJSON;
   using namespace BeeFishMisc;
   using namespace BeeFishPowerEncoding;


   inline bool test()
   {
   
      bool success = true;

      const Size min = 1;
      const Size max = 100;
      
      cout << "Test Database" << endl;

      const string filename = "/var/tmp/DBServer.data";
      remove(filename.c_str());

      Database db(filename);

      Path start(db);

      Path next = start["Hello"];

      next.setData("world");

      std::string world;
      start["Hello"].getData(world);

      success &=
         testValue(
            "world",
            world
         );


      // Min / Max
      Path data = start["data"];

      for (size_t i = min; i <= max; ++i)
      {
         data[i] = i;
      }

      if (success) {
         cerr << "\tTesting Data Path Min: " << flush;

         Stack stack;

         Size minimum =
            data.min<Size>(stack);

         cerr << "\tMinimum: " << minimum << flush;

         success = (minimum == min);

         outputSuccess(success);
      }

      if (success) {
         cout << "\tTesting Data Path Max: ";

         Stack stack;

         Size maximum =
            data.max<Size>(stack);

         success = (maximum == max);

         cerr << "\tMaximum: " << maximum << endl;
         outputSuccess(success);
      }

      if (success) {
         cout << "\tTesting Data Path Max through: ";

         data[max][0];
         data[max][1];
         success = (data.max<Size>() == max);
         outputSuccess(success);
      }

      if (success) {
         cout << "\tTesting Path Min/Max: ";

         (data.min<Size>() == min) &&
         (data.max<Size>() == max);

         outputSuccess(success);
      }

      if (success) {
         cout << "\tTesting get/set data: ";

         Path data = start;
         Size count = 22;
         data = 22;
         count = (Size)data;
         success =
            (count == 22);

         outputSuccess(success);
      }

      if (success) {
         cout << "\tTesting first ";
         Path data = start["skip"];
         data["first"];
         success =
            (data.min<string>() == "first");

         outputSuccess(success);
      }

      
      if (success) {
         cout << "\tTesting last ";
         Path data = start["skip2"];
         data["first"];
         data["zlast"];
         success =
            (data.min<string>() == "first") &&
            (data.max<string>() == "zlast");

         outputSuccess(success);
      }

      if (success) {
         cout << "\tTesting int first, next ";

         Path data = start["skip3"];
         data[0];
         data[1];
         //data[2];

         Stack stack;
         success =
            (data.min<int>(stack) == 0);
         int second = -1;
         success =
            data.next(stack, second);
 
         cout << "\t" << "data.next: " << success << endl;
         if (success)
            cerr << "Read second: " << second << endl;
         success &= (second == 1);
         outputSuccess(success);
      }

      if (success) {
         cout << "\tTesting string first, next ";

         Path data = start["skip4"];
         data["first"];
         data["second"];
         data["third"];

         Stack stack;
         success =
            (data.min<string>(stack) == "first");
         string second;
         success =
            data.next(stack, second);
 
         cout << "\t" << "data.next: " << success << endl;
         if (success)
            cerr << "Read second: \"" << second << "\"" << endl;
         success &= (second == "second");
         outputSuccess(success);
      }

      if (success)
      {
         cout << "\tTesting count" << endl;

         Path data = start["skip5"];
         for (int i = 1; i <= 10; ++i)
            data[i];

         Stack stack;
         int i = data.min<int>(stack);
         int check = 1;
         do {
            cerr << i << ",";
            success &= (i == check++);
         }
         while (data.next(stack, i));

         success &= (i == 10);
         outputSuccess(success);
      }

      remove(filename.c_str());

      outputSuccess(success);

      return success;
   }
   

}

#endif
