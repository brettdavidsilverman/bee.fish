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

      Path<Database::Encoding> start(db);

      Path next = start["Hello"];

      next.setData("world");

      std::string world;
      start["Hello"].getData(world);

      cout << "\t" << "Hello: " << world;
      success = (world == "world");
      outputSuccess(success);

      // Min / Max
      Path data = start["data"];

      for (size_t i = min; i <= max; ++i)
      {
         data[i] = i;
      }

      if (success)
      {
         data.setData("");
         cout << "\tTesting Data string" << flush;
         success = data.hasData();
         string value;
         data.getData(value);
         success = success && (value == "");
         outputSuccess(success);
      }

      if (success) {
         cout << "\tTesting Data Path Min: " << flush;

         Stack stack;

         Size minimum =
            data.min<Size>(stack);

         cout << minimum << flush;

         success = (minimum == min);

         outputSuccess(success);
      }

      if (success) {
         cout << "\tTesting Data Path Max: ";

         Stack stack;

         Size maximum =
            data.max<Size>(stack);

         success = (maximum == max);

         cout << maximum;
         outputSuccess(success);
      }

      if (success) {
         cout << "\tTesting Path Min/Max: ";

         (data.min<Size>() == min) &&
         (data.max<Size>() == max);

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
         cout << "\tTesting get/set data: ";

         Path<Database::Encoding> data = start;
         Size count = -1;
         data = 22;
         data.getData(count);
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
         cout << "\tTesting int next" << flush;

         Path data = start["skip3"];
         data[0];
         data[1];
         //data[2];

         Stack stack;
         int first = - 1;
         bool next =
            data.next(stack, first);
         if (next)
            cout << "\t\tFirst: " << first << "," << next << endl;
         success = (first == 0) && next;

         if (success) {
            int second = -1;
            next =
               data.next(stack, second);
            cout << "\t\t" << "Second: " << second << ", " << next << endl;
            success &= (second == 1) && next;
         }

         if (success) {
            int third = -1;
            next=
               data.next(stack, third);
 
            cout << "\t\t" << "End: " << third << ", " << next << endl;
            success &= (third == -1) && !next;
         }


         outputSuccess(success);
      }

      if (success) {
         cout << "\tTesting string first to next " << flush;

         Path data = start["skip4"];
         data["first"];
         data["second"];
         data["third"];

         Stack stack;
         success =
            (data.min<string>(stack) == "first");
         string second;
         bool next =
            data.next(stack, second);
 
         if (success) {
            cout << "\t\tRead second: \"" << second << "\", " << next << endl;
            success = (second == "second") && next;
         }

         if (success) {
            string third;
            next = data.next(stack, third);
            cout << "\t\tRead third: \"" << third << "\", " << next << endl;
            success = (third == "third") && next;
         }
         if (success) {
            string empty;
            next = data.next(stack, empty);
            cout << "\tString last next: " << next << endl;
            success = (next == false);
         }
         outputSuccess(success);
      }

      if (success)
      {
         cout << "\tTesting count" << flush;

         Path data = start["skip5"];
         for (int i = 1; i <= 10; ++i)
            data[i];

         Stack stack;
         int i;
         int check = 1;
         int last = data.max<int>();
         while (data.next(stack, i)) {
            cout << i;
            if (i != last)
               cout << ",";
            success &= (i == check++);
         }
         

         success &= (i == 10);
         outputSuccess(success);
      }

      if (success)
      {
         cout << "\tTesting string next" << flush;

         Path data = start["skip6"];
         data["one"];
         data["two"];
         data["three"];
         Stack stack;
         string key;
         string last;
         while (data.next(stack, key)) {
            cout << key << endl;
            last = key;
            key.clear();
         }
         

         success &= (last == "two");
         outputSuccess(success);
      }

      if (success)
      {
         cout << "\tTesting string value" << flush;

         Path data = start["skip7"];
         data["one"];
         data["two"];
         data["three"];
         string first = data.value<string>();
         success &= (first == "one");
         outputSuccess(success);
      }

      if (success)
      {
         cout << "\tTesting size_t contains" << flush;

         Path data = start["skip8"];
         Size size = 1;
         data[size];

         bool contains = data.contains(size);

         success &= (contains == true);
         outputSuccess(success);
      }

      if (success)
      {
         cout << "\tTesting size_t does not contain" << flush;

         Path data = start["skip9"];
         Size size = 1;
         data[size];

         bool contains = data.contains(Size(2));

         success &= (contains == false);
         outputSuccess(success);
      }

      remove(filename.c_str());

      outputSuccess(success);

      return success;
   }
   

}

#endif
