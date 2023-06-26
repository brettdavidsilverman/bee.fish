#ifndef BEE_FISH__DATABASE__TEST_HPP
#define BEE_FISH__DATABASE__TEST_HPP
#include "../Miscellaneous/Miscellaneous.hpp"

#include "../JSON/JSON.hpp"
#include "../Parser/Test.hpp"
#include "Database.hpp"
#include "../PowerEncoding/Stack.hpp"


namespace BeeFishDatabase
{
   using namespace BeeFishJSON;
   using namespace BeeFishMisc;
   using namespace BeeFishPowerEncoding;


   inline bool test()
   {
   
      bool success = true;

      cout << "Test Database" << endl;

      const string filename = "/var/tmp/temp.data";
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
      for (int i = 0; i <= 10; ++i)
      {
         data[i];
      }

      cout << "\tTesting Data Path Min: ";
      {
         PowerEncodingStack stack;
         data.min(stack);

         int count = 0;
         for (auto bit : stack)
         {
            if (bit) {
               ++count;
            }
            else {
               --count;
            }
         }

         Size minimum;
         stack >> minimum;
         success = success && (minimum == 0);
         success = success && (count == -1);

         outputSuccess(success);
      }

      cout << "\tTesting Data Path Max: ";
      {
         PowerEncodingStack stack;
         
         data.max(stack);

         int count = 0;
         for (auto bit : stack)
         {
            if (bit) {
               ++count;
            }
            else {
               --count;
            }
         }

         Size maximum;
         stack >> maximum;
  
         success = success && (maximum == 10);
         success = success && (count == -1);

         outputSuccess(success);
      }

      cout << "\tTesting Data Path Max through: ";
      {
         data[10][1];
         size_t max = data.max();
         success = success && (max == 10);
         outputSuccess(success);
      }

      cout << "\tTesting Path Min/Max: ";
      {
         success =
            (data.min() == 0) &&
            (data.max() == 10);

         outputSuccess(success);
      }

      cout << "\tTesting get/set data: ";
      {
         Path data = start;
         Size count = 22;
         data = 22;
         count = (Size)data;
         success =
            (count == 22);

         outputSuccess(success);
      }

      remove(filename.c_str());

      return success;
   }
   

}

#endif
