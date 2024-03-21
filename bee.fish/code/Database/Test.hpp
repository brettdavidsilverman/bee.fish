#ifndef BEE_FISH__DATABASE__TEST_HPP
#define BEE_FISH__DATABASE__TEST_HPP
#include "../Miscellaneous/Miscellaneous.hpp"

#include "Database.hpp"
#include "../test/test.h"

namespace BeeFishDatabase
{
   using namespace BeeFishJSON;
   using namespace BeeFishMisc;
   using namespace BeeFishTest;
   using namespace BeeFishPowerEncoding;

   inline bool testPath();
   inline bool testArray2Path();
   inline bool testSubArray2Path();
   inline bool testAllFiles(string url, string directory);
   inline bool testFile(Path root, string file, bool expect);
   

   inline bool test()
   {
   
      bool success = true;

      const Size min = 1;
      const Size max = 100;
      
      cout << "Test Database" << endl;
      
      Database db;

      Path start(db);

      Path next = start["Hello"];

      next.setData("world");

      std::string world;
      start["Hello"].getData(world);

      cout << "\t" << "Hello: " << world;
      success = (world == "world");
      outputSuccess(success);

      // Min / Max
      MinMaxPath data = start["data"];

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
         cout << "\tTesting Data Path Min through: ";

         data[min][0];
         data[min][1];
         Stack stack;
         success &= (data.min<Size>(stack) == min);
         success &= (stack.size() == 4);
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

         Path data = start;
         Size count = -1;
         data = 22;
         data.getData(count);
         success =
            (count == 22);

         outputSuccess(success);
      }

      if (success) {
         cout << "\tTesting first ";
         MinMaxPath data = start["skip"];
         data["first"];
         success =
            (data.min<string>() == "first");

         outputSuccess(success);
      }

      
      if (success) {
         cout << "\tTesting last ";
         MinMaxPath data = start["skip2"];
         data["first"];
         data["zlast"];
         success =
            (data.min<string>() == "first") &&
            (data.max<string>() == "zlast");

         outputSuccess(success);
      }

      if (success) {
         cout << "\tTesting int next" << endl;

         MinMaxPath data = start["skip3"];
         data[0];
         data[1];
         
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
         cout << "\tTesting three strings ";

         MinMaxPath data = start["skip4"];
         data["first"];
         data["second"];
         data["third"];

         Stack stack;
         vector<string> values;
         string value;
         while (data.next<string>(stack, value))
         {
            values.push_back(value);
         }
         success = (values.size() == 3);
         outputSuccess(success);
         
         if (success) {
            success = testValue("first", values[0]);
         }

         if (success) {
            success = testValue("second", values[1]);
         }
         
         if (success) {
            success = testValue("third", values[2]);
         }
         
         outputSuccess(success);
      }

      if (success)
      {
         cout << "\tTesting count" << flush;

         MinMaxPath data = start["skip5"];
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

         MinMaxPath data = start["skip6"];
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

         MinMaxPath data = start["skip7"];
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

         MinMaxPath data = start["skip8"];
         Size size = 1;
         data[size];

         bool contains = data.contains(size);

         success &= (contains == true);
         outputSuccess(success);
      }

      if (success)
      {
         cout << "\tTesting size_t does not contain" << flush;

         MinMaxPath data = start["skip9"];
         Size size = 1;
         data[size];

         bool contains = data.contains(Size(2));

         success &= (contains == false);
         outputSuccess(success);
      }
      
      if (success)
      {
         cout << "\tTesting nested keys" << flush;
          
         Path data = start["skip10"];
         data[1][1];
         Stack stack1;
         MinMaxPath test1 = data;
         int min1 = test1.min<int>(stack1);
         
         success &= (min1 == 1);
         
         data[1][2];
         Stack stack2;
         MinMaxPath test2 = data;
         int min2 = test2.min<int>(stack2);
         success &= (min2 == 1);
         
         MinMaxPath keys = data;
   
         int key;
         
         int count = 0;
         Stack stack;
         while (keys.next<int>(stack, key))
         {
            ++count;
         }
         
         success &= (count == 1);
         
         outputSuccess(success);
   
      }
     
      success = success &&
         testPath();
         
      success= success &&
         testArray2Path();
         
      success = success &&
         testSubArray2Path();
         
      success = success &&
         testAllFiles(HOST, TEST_DIRECTORY);
     
      db.close();
      remove(db.filename().c_str());

      outputSuccess(success);

      return success;
   }
   
   inline bool testPath()
   {
      using namespace std;

      cout << "Testing path" << endl;

      bool success = true;
      Database database;
      
      
      // Test string
      if (success)
      {
         JSON match;
         JSON2Path parser(database, match);
         parser.read("\"Hello World\"");
         
         cout << "\tParse string ";
         success = success &&
            parser.result() == true;
         BeeFishMisc::outputSuccess(success);
      }
      
      if (success) {
         
         cout << "\tIndexed string ";
         MinMaxPath path(database);

         success = success &&
            path.contains(Type::STRING);
            
         BeeFishMisc::outputSuccess(success);

      }
      
      if (success) {
         MinMaxPath path(database);
         path = path[Type::STRING];
         BeeFishScript::String value;
         path.getData(value);
         
         success = testValue("Hello World", value);
      }
      
      // Test array
      if (success)
      {
         JSON match;
         JSON2Path parser(database, match);
         parser.read("[1,[]]");
         
         cout << "\tParse array ";
         success &&
            parser.result() == true;
         BeeFishMisc::outputSuccess(success);
      }
      
      if (success) {
         
         cout << "\tIndexed array ";
         MinMaxPath path = database;
         success =
            path.contains(Type::ARRAY);
            
         BeeFishMisc::outputSuccess(success);

      }
      
      if (success) {
         MinMaxPath path = database;
         path = path[Type::ARRAY][0][Type::NUMBER];
         string value;
         path.getData(value);
         cout << "\tValue 1: " << value;
         success = (value == "1");
         BeeFishMisc::outputSuccess(success);
      }
      
      if (success) {
         cout << "\tSub Array: ";
         MinMaxPath path = database;
         success = path[Type::ARRAY].contains(1);
         BeeFishMisc::outputSuccess(success);
      }
      
      BeeFishMisc::outputSuccess(success);
      
      return success;
   }
   
   inline bool testArray2Path()
   {

      cout << "Test Array 2 Path: ";
 
      Database database;
      Path path = database;
      JSON match;
      JSON2Path parser(database, match);
      parser.read("[[]]");
      bool success = true;
      
      success = (parser.result() == true);
      
      if (success)
      {
          //success = path[ARRAY][0].contains(ARRAY);
      }
      
      if (success) {
         success = path.contains(Type::ARRAY);
      }
      
      if (success) {
         path = path[Type::ARRAY];
         success = path.contains(0);
      }
      
      if (success) {
         MinMaxPath maxPath = path;
         Size max = maxPath.max<Size>();
         success &= testResult("\tmax == 0", max == 0);
         for (Size i = 0; i <= max; ++i)
         {
            if (!path.contains(i))
            {
                success = false;
                break;
            }
         }
      }
      
      if (success) {
         path = path[0];
         success = path.contains(Type::ARRAY);
      }
      
      if (success) {
         path = path[Type::ARRAY];
         success = path.isDeadEnd();
      }
      
      BeeFishMisc::outputSuccess(success);
      
      return success;
   }
   
   inline bool testSubArray2Path()
   {

      cout << "Test Sub Array 2 Path: ";
      
      Database database;
      Path path = database;
      JSON match;
      JSON2Path parser(database, match);
      parser.read("[[1]]");
      bool success = true;
      
      success = testResult(
         "\tparser result",
         parser.result() == true
      );
      
      if (success)
      {
          //success = path[ARRAY][0].contains(ARRAY);
      }
      
      if (success) {
         cerr << "\tOuter Array: ";
         
         success = path.contains(Type::ARRAY);
         BeeFishMisc::outputSuccess(success);
      
      }
      
      if (success) {
         cerr << "\tOuter Array first index: ";
         path = path[Type::ARRAY];
         success = path.contains(0);
         BeeFishMisc::outputSuccess(success);
      }
      
      if (success) {
         cerr << "\tOuter Array max index: ";
         MinMaxPath maxPath = path;
         Size max = maxPath.max<Size>();
         success = (max == 0);
         BeeFishMisc::outputSuccess(success);
      }
      
      if (success) {
         cerr << "\tInner Array: ";
         path = path[0];
         success = path.contains(Type::ARRAY);
         BeeFishMisc::outputSuccess(success);
      }
      
      if (success) {
         path = path[Type::ARRAY];
      }
   
      if (success) {
         cerr << "\tInner Array Max index: ";
         MinMaxPath maxPath = path;
         if (path.isDeadEnd())
            success = false;
         else {
            Size max = maxPath.max<Size>();
            success = (max == 0);
         }
         BeeFishMisc::outputSuccess(success);
      }
      
      if (success) {
         
         path = path[0];
         success = path.contains(Type::NUMBER);
         BeeFishMisc::outputSuccess(success);
      }
      
      if (success) {
         Path valuePath =
            path[Type::NUMBER];
         std::string value;
         valuePath.getData(value);
         success = (value == "1");
         BeeFishMisc::outputSuccess(success);
      }
      
      BeeFishMisc::outputSuccess(success);
      
      return success;
   }
   
   inline bool testAllFiles(string url, string directory)
   {
      cout << "Testing all files in " << directory << endl;

      bool success = true;
      
      vector<string> files;

      for (const auto & entry : directory_iterator(directory))
      {
         files.push_back(entry.path());
      }

      sort(files.begin(), files.end());

      // Test direct to database
      Database tempDB;
         
      for (auto file : files) {
         if (success)
            success = testFile(tempDB, file, true);
         else
            break;
      }

      outputSuccess(success);

      return success;
   }
   
   inline bool testFile(Path root, string file, bool expect)
   {
      cout << "\t" << file << flush;
      
      string tempFile =
         TEMP_DIRECTORY;

      tempFile += "test.json";
      remove(tempFile);
      
      ifstream inputFile(file);
      JSON json;
      JSON2Path parser(root[file], json);
      parser.read(inputFile);
      parser.eof();
      inputFile.close();
      
      bool success = (parser.result() == true);
      
      if (success && expect)
      {
         ofstream outputFile(tempFile);
         Path2JSON path = root[file];
         outputFile << path;
         outputFile.close();
         
         // Compare the files
         success = success &&
            compareFiles(tempFile, file);
            
         if (!success) {
            ifstream inputFile(tempFile);
         
            string line;
            while (getline(inputFile, line))
               cout << line << endl;
            
            inputFile.close();
         }
         
         if (success)
            remove(tempFile);
 
      }
      
      outputSuccess(success);
      
      return success;
      
   }
   
   
   
}

#endif
