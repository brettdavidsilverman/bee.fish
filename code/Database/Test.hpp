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

   inline bool testNextIndex();
   inline bool testPath();
   inline bool testArray2Path();
   inline bool testSubArray2Path();
   inline bool testAllFiles(string url, string directory);
   inline bool testFile(Path root, string file, bool expect);
   

   inline bool testNextIndex()
   {
      using namespace std;

      cout << "Testing next index" << endl;

      JSONDatabase db("test");
 
      Index index1 = db.getNextIndex();
      Index index2 = db.getNextIndex();
 
      bool success =
        (index2 == index1 + sizeof(Branch));
        
      outputSuccess(success);
      
      return success;
   }

   inline bool test()
   {
   
      bool success = true;

      const Size min = 1;
      const Size max = 100;
      
      cout << "Test Database" << endl;
      
      success = success &&
         testNextIndex();
         
      if (!success)
         return false;
         
      JSONDatabase db("test");
      Path start(db);
      

      cout << "\tSimple path [] " << flush;

      cout << endl << "Start: " << start.index()  << endl;


      //cout << "Next: " << next.index() << endl;
      Index first, second;

      cout << "Hello 0: " << (first = start["Hello"].index())  << endl;
      cout << "Hello 1: " << (second = start["Hello"].index())  << endl;
      cout << "Hello 2: " << start["Hello"].index()  << endl;

      Path next = start["Hello"];

      success = (first == second && first == next.index());
      
      outputSuccess(success);

      if (!success)
         return false;

      if (success)
      {

            cout << "\tGet/Set Data size() " << flush;
            BString test = "world";
            next.setData(test);
            std::string testHelloData = next.getData();
            success = testHelloData.size() == strlen("world");
            outputSuccess(success);

            cout << "\tGet/Set Data str(): " << flush;
            std::string testHelloStr = testHelloData;

            success = (testHelloStr == "world");
            outputSuccess(success);
      }      

      if (success)
      {

            cout << "\tGet/Set Data size() " << flush;

            std::string
               testHello = next.getData();

            success = testHello.size() == strlen("world");
            outputSuccess(success);

            cout << "\tGet/Set Data str(): " << flush;
            std::string testHelloStr = testHello;
            success = (testHelloStr == "world");
            outputSuccess(success);
      }      

      std::string testNext1 = start["Hello"].getData();
      cout << "\t" << "Hello 1: " << testNext1.size() ;
      std::string world1 = testNext1;
      success = success && (world1 == "world");
      outputSuccess(success);

      std::string testNext2 = next.getData();
      std::string world2 = testNext2;
      cout << "\t" << "Hello 2: " << testNext2.size() ;
      success = success && (world2 == "world");
      outputSuccess(success);

      MinMaxPath data = start["data"];
      
      // Min / Max

      for (Size i = min; i <= max; ++i)
      {
         data[i].setData<Size>(i);
      }
      
      
      if (success)
      {
         cout << "\tTesting Contains " << flush;
         success = data.contains(100);
         outputSuccess(success);
      }
      
      if (success)
      {
         cout << "\tTesting Data string" << flush;
         
         data.setData("");
         
         success = !data.hasData();
         if (success)
         {
            BString value;
            data.getData(value);
            success = success && (value == "");
         }
         outputSuccess(success);
      }

      if (success) {
         cout << "\tTesting Data Path Min: " << flush;
         
         Size minimum =
            data.min<Size>();

         cout << minimum << flush;

         success = (minimum == min);

         outputSuccess(success);
         
         assert(success);
         
      }

      if (success) {
         cout << "\tTesting Data Path Max: ";

         Size maximum =
            data.max<Size>();

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
         success = success && (data.min<Size>() == min);
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

         Path data = start["getset"];
         Size count = -1;
         data.setData<Size>(22);
         data.getData<Size>(count);
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
         
         cout << "\t\tFirst: " << flush;
         
         Stack stack(data);
         int first = - 1;
         bool next =
            data.next(stack, first);
         
         
         success = (first == 0) && next;

         outputSuccess(success);

         if (success) {
            cout << "\t\t" << "Second: " << flush;
            int second = -1;
            next =
               data.next(stack, second);
            success &= (second == 1) && next;
            outputSuccess(success);
         }

         if (success) {
            int third = -1;
            next=
               data.next(stack, third);
 
            cout << "\t\t" << "End: " << third << ", " << next << endl;
            success &= (third == -1) && !next;
            outputSuccess(success);
         }


         outputSuccess(success);
      }

      if (success) {
         cout << "\tTesting three strings ";

         MinMaxPath data = start["skip4"];
         data["first"];
         data["second"];
         data["third"];

         Stack stack(data);
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

         Stack stack(data);
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
         Stack stack(data);
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
         cout << "\tTesting string min,latest,value " << flush;

         MinMaxPath data = start["skip7"];
         data["one"];
         data["two"];
         data["three"];
         string min = data.min<string>();
         success &= (min == "one");
         string latest = data.latest<string>();
         success &= (latest == "three");
         string value = data.value<string>();
         success &= (value == latest);
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
         cout << "\tTesting string contains" << flush;

         MinMaxPath data = start["skip8.1"];
         data["hello"];

         bool contains = data.contains("hello");

         success &= (contains == true);
         outputSuccess(success);
      }

      if (success)
      {
         cout << "\tTesting string does not contain" << flush;

         MinMaxPath data = start["skip9.1"];
         data["hello"];

         bool contains = data.contains("help");

         success &= (contains == false);
         outputSuccess(success);
      }

      if (success)
      {
         cout << "\tTesting nested keys" << flush;
          
         Path data = start["skip10"];
         data[1][1];
         data[1][2];
         
         Stack stack1(data);
         MinMaxPath test1 = data;
         int min1 = test1.min<int>(stack1);
         
         success &= (min1 == 1);
         
         outputSuccess(success);

         Stack stack2(data);
         MinMaxPath test2 = data;
         int min2 = test2.min<int>(stack2);

         success &= (min2 == 1);

         outputSuccess(success);

         MinMaxPath keys = data;
   
         int key;
         int count = 0;
         Stack stack(keys);
         while (keys.next<int>(stack, key))
         {
            ++count;
         }
         
         success &= (count == 1);
         
         outputSuccess(success);
   
      }
     
      if (success)
      {
         cout << "\tTesting latest string 1" << flush;

         MinMaxPath data = start["skip10.1"];
         data["hello"];
         data["hello world"];
         std::string string = data.latest<std::string>();

         success &= (string == "hello world");
         outputSuccess(success);

      }
      
      if (success)
      {
         cout << "\tTesting latest string 2" << flush;

         MinMaxPath data = start["skip10.2"];
         data["hello world"];
         data["hello"];
         
         std::string string = data.latest<std::string>();

         success &= (string == "hello");
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
     
      outputSuccess(success);

      return success;
   }
   
   
   
   inline bool testPath()
   {
      using namespace std;

      cout << endl << "Testing path" << endl;

      bool success = true;
      JSONDatabase database("test");
      Path root = database;
      cout << "\tTest data: " << std::flush;
      {
      
         BString str1 = "Hello 🤗";
         Path path = root["str"];
         path.setData(str1);
         Path path2 = root["str"];
         BString str2;
         path2.getData(str2);
      
         success = testValue(str1,str2);
         BeeFishMisc::outputSuccess(success);
      }
      
      // Test string
      cout << "\tTest strings" << endl;
      
      if (success)
      {
         MinMaxPath path = root["string"];
         JSONPathParser parser(path);
         parser.read("\"Hello World\"");
         parser.eof();
         cout << "\t\tParse string " << flush;
         success = success &&
            parser.result() == true;
         BeeFishMisc::outputSuccess(success);
         
      }
      
      if (success) {
         
         cout << "\tIndexed string ";
         MinMaxPath path = root["string"];
         
         success = success &&
            path.contains(Type::STRING);
            
         BeeFishMisc::outputSuccess(success);
      }
      
      if (success) {
         cout << "\tString value ";
         MinMaxPath path = root["string"];
         path = path[Type::STRING];
         BString value;
         path.getData(value);
         
         success = testValue("Hello World", value);
         
         BeeFishMisc::outputSuccess(success);
      }
      
      cout << "Test arrays \"[1,[]]\"" << endl;
      if (success)
      {
         Path path = root["array"];
         JSONPathParser parser(path);
         parser.read("[1,[]]");
         parser.eof();
        
         cout << "\tParse array ";
         success &&
            parser.result() == true;
         BeeFishMisc::outputSuccess(success);
      }
      
      if (success) {
         
         cout << "\tIndexed array ";
         Path path = root["array"];
         success =
            path.contains(Type::ARRAY);
            
         BeeFishMisc::outputSuccess(success);

      }
      
      if (success) {
         cout << "\tItem 0: ";
         MinMaxPath path = root["array"][Type::ARRAY];
         
         Size test = MinMaxPath(path).min<Size>();
         success = success &&
            testResult("Item 0 index", test == 0);
         success = success && 
            path.contains(test);
         BeeFishMisc::outputSuccess(success);
      }
      
      if (success) {
         cout << "\tType item 0 ";
         Path path = root["array"][Type::ARRAY][0];
         success = path.contains(Type::INTEGER);
         BeeFishMisc::outputSuccess(success);
      }
      
      if (success) {
         cout << "\tValue item 0 ";
         Path path = root["array"][Type::ARRAY][0][Type::INTEGER];
         BString value;
         path.getData(value);
         success = (value == "1");
         BeeFishMisc::outputSuccess(success);
      }
      
      if (success) {
         cout << "\tItem 1 ";
         Path path = root["array"];
         path = path[Type::ARRAY];
         success = path.contains(1);
         BeeFishMisc::outputSuccess(success);
      }
      
      if (success) {
         cout << "\tType item 1 ";
         Path path = root["array"][Type::ARRAY][1];
         success = path.contains(Type::ARRAY);
         BeeFishMisc::outputSuccess(success);
      }
      
      if (success) {
         cout << "\tValue item 1 ";
         Path path = root["array"];
         success = path[Type::ARRAY][1][Type::ARRAY].isDeadEnd();
         BeeFishMisc::outputSuccess(success);
      }
      

      cout << "Test numbers" << endl;
      if (success) {
         cout << "\tInteger" << flush;
         Path path = root["integer"];
         JSONPathParser parser(path);
         parser.read("1234");
         parser.eof();
         success = (parser.result() == true);
         BeeFishMisc::outputSuccess(success);
         assert(success);
      }
      
      if (success)
      {
         cout << "\tInteger type" << endl;
         MinMaxPath path = root["integer"];
         
         Stack stack(path);
         success = path.contains(Type::INTEGER);
         BeeFishMisc::outputSuccess(success);
      }
      
      if (success)
      {
         cout << "\tInteger value" << endl;
         Path path = root["integer"][Type::INTEGER];
         BString value;
         path.getData(value);
         success = (value == "1234");
         BeeFishMisc::outputSuccess(success);
      }
      
      BeeFishMisc::outputSuccess(success);
      
      return success;
   }
   
   inline bool testArray2Path()
   {

      cout << "Test Array 2 Path \"[[]]\" " << endl;
 
      JSONDatabase database("test");
      Path path = Path(database)["array"];
      JSONPathParser parser(path);
      parser.read("[[]]");
      parser.eof();
      bool success = true;
      
      success = (parser.result() == true);
      BeeFishMisc::outputSuccess(success);
      
      if (success) {
         success = path.contains(Type::ARRAY);
      }
      
      if (success) {
         path = path[Type::ARRAY];
         success = path.contains(0);
      }
      
      if (success) {
         MinMaxPath maxPath = path;
         Stack stack(maxPath);
         Size max = maxPath.max<Size>();
         success &= testResult("\tmax == 0", max == 0);
         assert(success);
         
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
      
      JSONDatabase database("test");
      Path path = database;
      JSONPathParser parser(path);
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
         cout << "\tOuter Array: ";
         
         success = path.contains(Type::ARRAY);
         BeeFishMisc::outputSuccess(success);
      
      }
      
      if (success) {
         cout << "\tOuter Array first index: ";
         path = path[Type::ARRAY];
         success = path.contains(0);
         BeeFishMisc::outputSuccess(success);
      }
      
      if (success) {
         cout << "\tOuter Array max index: ";
         MinMaxPath maxPath = path;
         Size max = maxPath.max<Size>();
         success = (max == 0);
         BeeFishMisc::outputSuccess(success);
      }
      
      if (success) {
         cout << "\tInner Array: ";
         path = path[0];
         success = path.contains(Type::ARRAY);
         BeeFishMisc::outputSuccess(success);
      }
      
      if (success) {
         path = path[Type::ARRAY];
      }
   
      if (success) {
         cout << "\tInner Array Max index: ";
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
         cout << "\tInner array contains number" << endl;
         path = path[0];
         success = path.contains(Type::INTEGER);
         BeeFishMisc::outputSuccess(success);
      }
      
      if (success) {
         cout << "\tInner array value" << endl;
         path = path[Type::INTEGER];
         BString value;
         path.getData(value);
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
      JSONDatabase tempDB("temp");
         
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
      cout << "\t" << file << endl;
      
      string tempFile =
         TEMP_DIRECTORY;

      tempFile += "test.json";
      remove(tempFile);
      
      ifstream inputFile(file);
      JSONPathParser parser(root[file]);
      parser.read(inputFile);
      parser.eof();
      inputFile.close();
      
      bool success = (parser.matched());
      
      if (success && expect)
      {
         ofstream outputFile(tempFile);
         JSONPath path(root[file]);
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
