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

    inline bool testFile();
    inline bool testNextIndex();
    inline bool testPath();
    inline bool testJSONPath();
    inline bool testArray2Path();
    inline bool testSubArray2Path();
    inline bool testComplexArrays();
    inline bool testAllFiles(std::filesystem::path directory);
    inline bool testFile(JSONPath root, std::filesystem::path file, bool expect);
    inline bool testIterator();
    inline bool testClear();
    inline bool testPathParent();
    inline bool testIncrement();
    inline bool testJSONPathParent();
    inline bool testDeleteProperty();
    inline bool testObjects();
    inline bool testMultiThreaded();
    
    inline bool test()
    {
        bool success = true;
        //return testMultiThreaded();
        
        cout << "Test Database " << endl;

        success = success &&
            testFile();
            
        success = success &&
            testNextIndex();
        
        success = success &&
            testPath();
                    
        success = success &&
            testJSONPath();
            
        success= success &&
            testArray2Path();
            
        success = success &&
            testSubArray2Path();
            
        success = success &&
            testClear();
            
        success = success &&
            testPathParent();
            
        success = success &&
            testJSONPathParent();
            
        success = success &&
            testIncrement();
            
        success = success &&
            testDeleteProperty();
            
        success = success &&
            testIterator();
            
        success = success &&
            testObjects();
            
        success = success &&
            testMultiThreaded();
            
        success = success &&
            testAllFiles(TEST_DIRECTORY);

            
        outputSuccess(success);

        return success;
    }
    
    
    inline bool testFile()
    {
        cout << "Test file " << endl;
        bool success = true;
        
        LockFile file;

        cout << "\tLock" << endl;
        file.lock();
        file.lock();
        
        cout << "\tUnlock" << endl;
        file.unlock();
        file.unlock();
        
        BString test = "Hello world";
        file.write(test.data(), test.size());
        
        file.seek(0);
        test = BString(test.size(), '\0');
        
        file.read(test.data(), test.size());
        
        success = testValue(
            "File read/write",
            test == "Hello world"
        );
        
        if (success)
        {
            cout << "\tLock index 1" << flush;
            Database db;
            db.lock();
            db.unlock();
            Database db2(db.filename());
            db2.lock();
            db2.unlock();
            outputSuccess(true);
            
            {
                cout << "\tLock index 2" << flush;
                {
                    LockFile::ScopedLock lock1(db);
                    LockFile::ScopedLock lock2(db);
                }
                outputSuccess(true);
            }
            
            
            cout << "\tLock index3" << flush;
            Path path1 = db;
            Path path2 = db2;
            {
                LockFile::ScopedLock lock1(db);
            }
            {
                LockFile::ScopedLock lock2(db2);
            }
            outputSuccess(true);
        }
        
        if (success)
        {
            JSONDatabase db;

            BString value = "101";
            Index data = db.allocate(value);
            
            success = success &&
                testValue(
                    "Allocated",
                    data > 0
                );
                
            BString value2;
            JSONPath path = db.json();
            
            path.setData(value);
            value2 = path.getStringData();
            
            success = success &&
                testValue(
                    "Get/set data",
                    value == value2
                );
                
            path.setData<BString>(value);
            value2 = path.getStringData();
            
            success = success &&
                testValue(
                    "Get/set data 2",
                    value == value2
                );
            
            JSONPathParser parser(path);
            parser.read("101");
            parser.eof();
            assert(parser.matched());
            
            std::stringstream stream;
            stream << path;
    
            success = success &&
                testValue(
                    "Database read/write",
                    BString(stream.str()) == "101"
                );
            
            
        }

        outputSuccess(success);

        return success;
    }
    
    
    inline bool testNextIndex()
    {
        using namespace std;

        cout << "Testing next index" << endl;

        
        
        Database db;
 
        cout << "\tIndex 1 " << flush;
        Index index1 = db.getNextIndex(0);
        cout << index1 << endl;
        
        cout << "\tIndex 2 " << flush;
        Index index2 = db.getNextIndex(0);
        cout << index2 << endl;
        
        bool success =
          (index2 == index1 + sizeof(Branch));
          
        outputSuccess(success);
        
        return success;
    }

    inline bool testPath()
    {
    
        bool success = true;

        const Size min = 1;
        const Size max = 100;
        
        cout << "Test Path" << endl;
        
        
            
        Database db;
        
        Path start = db;
        

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
                std::string testHelloData = next.getStringData();
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
                    testHello = next.getStringData();

                success = testHello.size() == strlen("world");
                outputSuccess(success);

                cout << "\tGet/Set Data str(): " << flush;
                std::string testHelloStr = testHello;
                success = (testHelloStr == "world");
                outputSuccess(success);
        }        

        std::string testNext1 = start["Hello"].getStringData();
        cout << "\t" << "Hello 1: " << testNext1.size() ;
        std::string world1 = testNext1;
        success = success && (world1 == "world");
        outputSuccess(success);

        std::string testNext2 = next.getStringData();
        std::string world2 = testNext2;
        cout << "\t" << "Hello 2: " << testNext2.size() ;
        success = success && (world2 == "world");
        outputSuccess(success);

        Path data = start["data"];
        
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
                BString value = data.getStringData();
                
                success = success &&
                    testValue(
                        "Get empty data 1",
                        value == ""
                    );
            }
        
            
            
            if (success)
            {
                BString value;
                value = data.getStringData();
                success = success &&
                    testValue(
                        "Get empty data 2",
                        value == ""
                    );
            }
            
            if (success)
            {
                BString value =
                data.getStringData();
                success = success &&
                    testValue(
                        "Get empty data 3",
                        value == ""
                    );
            }
            
            assert(success);
            

        }
        
        Path bookmark = data;
        
        if (success) {
            cout << "\tTesting Data Path goToMin 1: " << flush;
            
            Size minimum = -1;
            Stack stack;
            bool ok = data.goToMin(stack);

   
            if (ok) {
                stack >> minimum;
                cout << minimum << flush;
            }

            success = ok && (minimum == min);
            
            outputSuccess(success);
            
            assert(success);
            
            data = bookmark;
            
        }
        
        if (success) {
            cout << "\tTesting Data Path goToMin 2: " << flush;
    
            Size minimum = -1;
            Stack stack;
            bool ok = data.goToMin(stack);

   
            if (ok) {
                stack >> minimum;
                cout << minimum << flush;
            }

            success = ok && (minimum == min);
            
            outputSuccess(success);
            
            assert(success);
            data = bookmark;
        }

        if (success) {
            cout << "\tTesting Data Path Min: " << flush;
            
            Size minimum = -1;
            data.reset();
            bool ok = data.min<Size>(minimum);

            if (ok)
                 cout << minimum << flush;

            success = ok && (minimum == min);

            outputSuccess(success);
            
            assert(success);
            
        }

        if (success) {
            cout << "\tTesting Data Path Max: " << flush;


            Size maximum = 0;
            data.max<Size>(maximum);

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
            count = data.getData<Size>();
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
            cout << "\tTesting int next" << endl;


            Path data = start["skip3"];
            data[0];
            data[1];

            cout << "\t\tFirst: " << flush;
            
            Stack stack;
            int first = - 1;

            bool next =
                data.next(stack, first);

            success = (first == 0) &&
                next && 
                data.PowerEncoding::count() == 0;
            
            outputSuccess(success);

            if (success) {
                cout << "\t\t" << "Second: " << flush;
                int second = -1;
                next =
                    data.next(stack, second);
    cerr << stack << endl;
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

            Path data = start["skip4"];
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

        if (success)
        {
            cout << "\tTesting string contains" << flush;

            Path data = start["skip8.1"];
            data["hello"];

            bool contains = data.contains("hello");

            success &= (contains == true);
            outputSuccess(success);
        }

        if (success)
        {
            cout << "\tTesting string does not contain" << flush;

            Path data = start["skip9.1"];
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
            

            Path test1 = data;
            int min1 = test1.min<int>();
            
            success &= (min1 == 1);
            
            outputSuccess(success);

            Path test2 = data;
            int min2 = test2.min<int>();

            success &= (min2 == 1);

            outputSuccess(success);

            Path keys = data;
    
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
      
        
        outputSuccess(success);
        
        return success;
    }
    
    
    inline bool testJSONPath()
    {
        using namespace std;

        cout << endl << "Testing json path" << endl;

        bool success = true;
        
        cout << "\tTest type" << endl;
        JSONDatabase database;
        
        JSONPath start = database.host("https://test");
        JSONPath test =start["test"];
        
        test.setUndefined();
        
        success = success && 
            testValue(
                "Set to undefined",
                test.type() == Type::UNDEFINED
            );
        
            /*
        if (success) {
            cout << "\tKeyed \"hello\" json path" << endl;
            start["hello"].lock();
            start["hello"].lock();
            success = success && 
            testValue(
                "Hello Unlocked",
                true
            );
            
            
        }
        */
        
        JSONPath root = database.host("https://test");

        
        // Test string
        cout << "\tTest strings" << endl;
        
        if (success)
        {
            JSONPath path = root["string"];
            JSONPathParser parser(path);
            cout << "\t\tParse string 2" << flush;
            parser.read("\"Hello World\"");
            parser.eof();
            
            success = success &&
                parser.result() == true;
            BeeFishMisc::outputSuccess(success);
            
        }
        
        if (success) {
            
            cout << "\tIndexed string ";
            Path path = root["string"];
            Type type =
            path.getData<Type>();
            success = success &&
                type == Type::STRING;
                
            BeeFishMisc::outputSuccess(success);
        }
        
        if (success) {
            cout << "\tString value ";
            Path path = root["string"];
            success = success &&
                path[JSONPath::VALUE].hasData();
                
            if (success)
            {
                BString value =
                    path[JSONPath::VALUE].getStringData();
            
                success = testValue("Hello World", value);
            }
            
            BeeFishMisc::outputSuccess(success);
        }
    
    
        cout << "Test arrays \"[1,[]]\"" << endl;
        if (success)
        {
            JSONPath path = root["array"];
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
            success =
                root["array"].type() == Type::ARRAY;
                
            BeeFishMisc::outputSuccess(success);

        }
        
        if (success) {
            cout << "\tItem 1:" << endl;
            Path path = root["array"];
    
            Size test = path[JSONPath::CHILDREN].min<Size>();
            success = success &&
                testResult("\tItem 1 index", test == 1);
                
            success = success && 
                testResult("\tItem contains 1", path[JSONPath::CHILDREN].contains(test));
            
        }
        
        if (success) {
            cout << "\tType item 1 ";
            Path path = root["array"];
            path = path[JSONPath::CHILDREN][1];
            Type type =
                path.getData<Type>();
            success = (type == Type::INTEGER);
            BeeFishMisc::outputSuccess(success);
        }
        
        if (success) {
            cout << "\tValue item 1 ";
            Path path = root["array"];
            path = path[JSONPath::CHILDREN][1];
            BString value =
                path[JSONPath::VALUE].getStringData();
            success = (value == "1");
            BeeFishMisc::outputSuccess(success);
        }
        
        if (success) {
            cout << "\tItem 2 ";
            Path path = root["array"];
            path = path[JSONPath::CHILDREN];
            success = path.contains(2);
            BeeFishMisc::outputSuccess(success);
        }
        
        if (success) {
            cout << "\tType item 2 ";
            Path path = root["array"];
            path = path[JSONPath::CHILDREN][2];
            Type type =
                path.getData<Type>();
            success = (type == Type::ARRAY);
            BeeFishMisc::outputSuccess(success);
        }
        
        if (success) {
            cout << "\tValue item 2 ";
            Path path = root["array"];
            success = path[JSONPath::CHILDREN][2][JSONPath::CHILDREN].isDeadEnd();
            BeeFishMisc::outputSuccess(success);
        }
        

        cout << "Test numbers" << endl;
        if (success) {
            cout << "\tInteger" << flush;
            JSONPath path = root["integer"];
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
            Path path = root["integer"];
            
            Type type =
                path.getData<Type>();
            success = (type == Type::INTEGER);
            BeeFishMisc::outputSuccess(success);
        }
        
        if (success)
        {
            cout << "\tInteger value" << endl;
            Path path = root["integer"];
            BString value =
                path[JSONPath::VALUE].getStringData();
            success = (value == "1234");
            BeeFishMisc::outputSuccess(success);
        }
        
                
        if (success)
        {
            JSONPath path = root["tobecleared"];
            JSONPathParser parser(path);
            cout << "\tParse string 3" << flush;
            parser.read("\"Hello World\"");
            parser.eof();

            success = success &&
                parser.result() == true;
            BeeFishMisc::outputSuccess(success);
            
            BString string1 = path.toString();
cerr << string1 << endl;
            path.clear();

            success = success &&
                testValue(
                    "Cleared json is dead end",
                     path.isDeadEnd()
                );

            path.setUndefined();

            success = success &&
                testValue(
                    "New type is undefined", 
                    (path.type() == Type::UNDEFINED)
                );
            
            BString string2 = path.toString();

            success = success &&
                testValue(
                    "Cleared to string =",
                    string1 == string2
                );

        }
        
        BeeFishMisc::outputSuccess(success);
        
        return success;
    }
    
        
    
    inline bool testArray2Path()
    {

        cout << "Test Array 2 Path \"[[]]\" " << endl;
 
        JSONDatabase database;
        JSONPath json =
            database.json()["array"];
        JSONPath _path = database.json()["array"];
        JSONPathParser parser(_path);
        Path path = _path;
        parser.read("[[]]");
        parser.eof();
        bool success = true;
        
        success = (parser.result() == true);
        BeeFishMisc::outputSuccess(success);
        
        if (success) {
            success = _path.type() == Type::ARRAY;
        }
        
        if (success) {
            path = path[JSONPath::CHILDREN];
            success = path.contains(1);
        }
        
        if (success) {
            Path maxPath = path;
            Stack stack;
            Size max = maxPath.max<Size>();
            success &= testResult("\tmax == 1", max == 1);
            assert(success);
            
            for (Size i = 1; i <= max; ++i)
            {
                if (!path.contains(i))
                {
                     success = false;
                     break;
                }
            }
        }
        
        if (success) {
            path = path[1];
            Type type =
                path.getData<Type>();
            success = (type == Type::ARRAY);
        }
        
        if (success) {
            path = path[JSONPath::CHILDREN];
            success = path.isDeadEnd();
        }
        
        BeeFishMisc::outputSuccess(success);
        
        return success;
    }
    
    inline bool testSubArray2Path()
    {

        cout << "Test Sub Array 2 Path: ";
        
        JSONDatabase database;
        JSONPath _path = database.host("https://test");
        JSONPathParser parser(_path);
        parser.read("[[1]]");
        Path path = _path;
        bool success = true;
        
        success = testResult(
            "\tparser result",
            parser.result() == true
        );
        
        JSONPath test = path;
        ;

        if (success)
        {
            std::stringstream stream;
            stream << test;
             //success = path[ARRAY][0].contains(ARRAY);
        }
        
        if (success) {
            cout << "\tOuter Array: ";
            Type type =
                path.getData<Type>();
            success = (type == Type::ARRAY);
            BeeFishMisc::outputSuccess(success);
        
        }
        
        if (success) {
            cout << "\tOuter Array first index: ";
            path = path[JSONPath::CHILDREN];
            success = path.contains(1);
            BeeFishMisc::outputSuccess(success);
        }
        

        
        if (success) {
            cout << "\tOuter Array max index: ";
            Stack stack;
            Index max = path.max<Index>();
            
            success = (max == 1);

            BeeFishMisc::outputSuccess(success);
        }
        
        if (success) {
            cout << "\tInner Array: ";
            path = path[1];
            Type type =
                path.getData<Type>();
            success = (type == Type::ARRAY);
            BeeFishMisc::outputSuccess(success);
        }
        
        if (success) {
            path = path[JSONPath::CHILDREN];
        }
    
        if (success) {
            cout << "\tInner Array Max index: ";
            Path maxPath = path;
            if (path.isDeadEnd())
                success = false;
            else {
                Size max = maxPath.max<Size>();
                success = (max == 1);
            }
            BeeFishMisc::outputSuccess(success);
        }
        
        if (success) {
            cout << "\tInner array contains number" << endl;
            path = path[1];
            Type type =
                path.getData<Type>();
            success = (type == Type::INTEGER);
            BeeFishMisc::outputSuccess(success);
        }
        
        if (success) {
            cout << "\tInner array value" << endl;
            BString value =
            path[JSONPath::VALUE].getStringData();
            success = (value == "1");
            BeeFishMisc::outputSuccess(success);
        }
        
        BeeFishMisc::outputSuccess(success);
        
        return success;
    }
    
    inline bool testComplexArrays()
    {
            
        cout << "Test complex arrays: ";
        
        auto test =
        [](BString json) 
        {
        
            cout << json << endl;
            
            JSONDatabase database;
            JSONPath path = database.json();
            JSONPathParser parser(path);
        
            parser.read(json);
            parser.eof();
            bool success = true;
        
            success = testResult(
                "\tparser result",
                parser.result() == true
            );
            
            if (success) {
                stringstream stream;
                stream << path;
                success = success && 
                    testResult(
                        "\tCompare",
                        stream.str() == json
                    );
                if (!success) {
                    cout << stream.str();
                }
            }
            
            BeeFishMisc::outputSuccess(success);
            
            return success;
        };
        
        bool success = true;
        success = success && test(
"[\r\n"
"   1,\r\n"
"   2,\r\n"
"   [],\r\n"
"   [\r\n"
"      3,\r\n"
"      4\r\n"
"   ]\r\n"
"]"
        );
        
        success = success && test(
"[{\r\n"
"   \"a\": \"b\"\r\n"
"}]"
        );
        
        success = success && test(
"{\r\n"
"   \"a\": [1]\r\n"
"}"
        );
        
        success = success && test(
"{\r\n"
"   \"a\": \"b\",\r\n"
"   \"c\": \"d\"\r\n"
"}"
        );
        
        success = success && test(
"{\r\n"
"   \"a\": {\r\n"
"      \"a\": \"b\",\r\n"
"      \"c\": \"d\"\r\n"
"   }\r\n"
"}"
        );
        
        
        BeeFishMisc::outputSuccess(success);
assert(success);
        return success;
    }
    
    inline bool testAllFiles(std::filesystem::path directory)
    {
        cout << "Testing all files in " << directory.filename() << endl;

        bool success = true;
        
        vector<std::filesystem::path> files;

        for (const auto & entry : directory_iterator(directory))
        {
            files.push_back(entry);
        }

        sort(files.begin(), files.end());

        // Test direct to database
        JSONDatabase tempDB;
            
        for (auto file : files) {
            if (success)
                success = testFile(tempDB.host("https://test"), file, true);
            else
                break;
        }

        outputSuccess(success);

        return success;
    }
    
    inline bool testFile(JSONPath root, std::filesystem::path file, bool expect)
    {
        cout << "\t" << file.filename() << " " << flush;
        
        std::stringstream stream;
        stream << TEMP_DIRECTORY << &root << "test.json";
        
        string tempFile = stream.str();

        remove(tempFile);
        
        ifstream inputFile(file);
        JSONPath path = root[file.filename()];
        JSONPathParser parser(path);
        parser.read(inputFile);
        parser.eof();
        inputFile.close();
        
        bool success = (parser.matched());
        
        if (success && expect)
        {
            ofstream outputFile(tempFile);
            JSONPath path(root[file.filename()]);
            outputFile << path;
            outputFile.close();
            
            // Compare the files
            success = success &&
                compareFiles(file, tempFile);

            if (success)
                remove(tempFile);
 
        }
        
        outputSuccess(success);
        
        return success;
        
    }
    
    inline bool testIterator()
    {
        using namespace std;

        cout << "Testing iterator" << endl;
        
        bool success = true;
        
        Database db;
        Path root(db);
        
        Iterable<BString> path(root);
        
        root["hello"];
        root["world"];
        root["brett"];
        
        
        if (success) {
            cout << "\tPath min value 1: ";
        
            success = success && (path.min<BString>() == "brett");
            outputSuccess(success);
        }
        
        if (success) {
            cout << "\tPath min value 2: ";
            success = success && (path.min<BString>() == "brett");
            outputSuccess(success);
        }
        
        if (success) {
            cout << "\tPath max value 1: ";
        
            success = success && (path.max<BString>() == "world");
            outputSuccess(success);
        }
        
        if (success) {
            cout << "\tPath max value 2: ";
            success = success && (path.max<BString>() == "world");
            outputSuccess(success);
        }
        
        
        if (success)
        {
            cout << "\tIterable values forward: ";
            vector<BString> values;
        
            for (auto value : path)
            {
                values.push_back(value);
            }
        
            success = (values.size() == 3);
                
            success = success && (values[0] == "brett");
            success = success && (values[1] == "hello");
            success = success && (values[2] == "world");
        
            outputSuccess(success);
        }
        
        if (success)
        {
            cout << "\tIterable values reverse: ";
            vector<BString> values;
        
            for (auto it = path.rbegin(); it != path.rend(); ++it)
            {
                values.push_back(*it);
            }
        
            success = (values.size() == 3);
                
            success = success && (values[0] == "world");
            success = success && (values[1] == "hello");
            success = success && (values[2] == "brett");
        
            outputSuccess(success);
        }
        
        return success;
        
    }
    
    void iterate(Database& db, Index index, Index tabs = 0)
    {
        Branch branch = db.getBranch(index);
        for (Index  i = 0; i < tabs; ++i)
            cerr << "    ";
        cerr << index << endl;
        if (branch._left)
        {
            cerr << "LEFT ";
            iterate(db, branch._left, tabs + 1);
        }
        else if (branch._right)
        {
            cerr << "RIGHT ";
            iterate(db, branch._right, tabs + 1);
        }
    };

    
    inline bool testClear()
    {

        cout << "Test clear" << endl;
        
        bool success = true;
 
             
        Index key1 = 0;
        Index key2 = 1;
        
        Database database2;
        Path path2 = database2;
        path2[key1];
        path2[key2];
        path2.clear(key1);
        success = success &&
            testValue(
                "Key 1 cleared",
                !path2.contains(key1)
            );
            
        success = success &&
            testValue(
                "Key 2 remains",
                path2.contains(key2)
            );
            
        if (success) 
            path2.clear(key2);
                        
        success = success &&
            testValue(
                "Key 2 cleared",
                !path2.contains(key2)
            );
            

        success = success &&
            testValue(
                "Path cleared to dead end",
                path2.isDeadEnd()
            );
            
        Database database;
        Path path = database;
        BString key = "hello";
        path[key];
        path["hell"];
        
        success = testValue(
            "Path contains",
            path.contains(key)
        );
        
        if (success)
            path.clear(key);
            
        success = success &&
            testValue(
                "Single Path cleared",
                !path.contains(key)
            );
            
        success = success &&
            testValue(
                "Single Path contains",
                path.contains("hell")
            );
            
        if (success)
        {
            Database database;
            Path path = database;
            for (Index i = 0; i < 100; ++i)
                path[i];
            
            Iterable<Index> iterable(path);
            for (int i = 99; i >= 0; --i)
            {
                path.clear(i);
            }
        
            success = success &&
                testValue(
                    "Path numbers is dead end",
                    path.isDeadEnd()
                );
                       
            if (!success)
                iterate(database, path.index());
           
        }
        
        if (success)
        {
            Database database;
            Path path = database;
            for (Index i = 0; i < 10; ++i)
                for (Index a = 0; a < 10; ++a)
                    path[i][a];
            
            Iterable<Index> iterable(path);
            for (int i = 9; i >= 0; --i)
            {
                for (int a = 0; a < 10; ++a)
                    path[i].clear(a);
                success = (path[i].isDeadEnd());
                if (!success)
                    break;
                path.clear(i);
            }
            
            success = testValue(
                "Complex path is dead end",
                path.isDeadEnd()
            );
        
            success = success &&
                testValue(
                    "Path numbers is dead end",
                    path.isDeadEnd()
                );
                       
            if (!success)
                iterate(database, path.index());
           
        }
        
        if (success)
        {
            Database database;
            Path path = database;
            path[key1];
            path[key2];
            path.clear();
            success = success &&
                testValue(
                    "Path clear is dead end",
                    path.isDeadEnd()
                );
                
            if (success) {
                path[key1][key1];
                path[key1].clear();
            }
            
            success = success &&
                testValue(
                    "Path contains key",
                    path.contains(key1)
                );
                
            success = success &&
                testValue(
                    "Path clear 2 is dead end",
                    path[key1].isDeadEnd()
                );
        }
        

        BeeFishMisc::outputSuccess(success);
        
        
        
        return success;
    }
    
    inline bool testPathParent()
    {

        cout << "Test parent" << endl;
        
        bool success = true;
 
        Database database;
        Path path = database;
        path = path["hello"]["world"];
        
        Stack stack;
        path = path.parent(stack);
        stack.reset();
        
    
        BString world;
        stack >> world;
    
        success = success &&
            testValue(
                "World",
                world == "world"
            );
            
        stack.clear();
        
        path = path.parent(stack);
        stack.reset();
        
        BString hello;
        stack >> hello;
        
        success = success &&
            testValue(
                "Hello",
                hello == "hello"
            );
            
        success = success &&
            testValue(
                "Path root",
                path.index() == 0
            );
            
        BString value;
        path = path["hello"]["world"];
        path = path.parent(value);
        
        success = success &&
            testValue(
                "World value",
                value == "world"
            );
            
        value.clear();
        path = path.parent(value);
        
        success = success &&
            testValue(
                "Hello value",
                value == "hello"
            );
            
        success = success &&
            testValue(
                "Path value root",
                path.index() == 0
            );
            
        BeeFishMisc::outputSuccess(success);
        
        return success;
    }
    
    inline bool testJSONPathParent()
    {

        cout << "Test JSON parent" << endl;
        
        bool success = true;
 
        JSONDatabase database;
        
        JSONPath start = database.host("https://test");
        JSONPathParser parser1(start);
        parser1.read("[1,2,3]");
        JSONPath path = start[Index(1)];

        BString key;
        JSONPath parent;
        parent = path.parent(key);

        success = success &&
            testValue(
                "Key 1",
                key == "1"
            );
            
        success = success &&
            testValue(
                "Parent is root",
                parent == start
            );
            
        start.clear();
    
        JSONPathParser parser2(start);
        parser2.read("{\"hello\":\"world\"}");
    
        path = start["hello"];
        key.clear();
        parent = path.parent(key);

        success = success &&
            testValue(
                "Key hello",
                key == "hello"
            );
            
        success = success &&
            testValue(
                "Parent is root",
                parent == start
            );
            
        
        BeeFishMisc::outputSuccess(success);
        
        return success;
    }
    
    inline bool testIncrement()
    {
        cout << "Test increment" << endl;
        
        Database database;
        Path path = database;
        
        bool success = true;
        
        Index count = path++;
        
        success = success &&
            testValue("Increment++ 0", count == 0);
            
        count = path++;
        success = success &&
            testValue("Increment++ 1", count == 1);
            
        count = ++path;
        success = success &&
            testValue("++Increment 3", count == 3);
            
        count = path--;
        success = success &&
            testValue("Increment-- 3", count == 3);
            
        count = --path;
        success = success &&
            testValue("--Increment 1", count == 1);
            
            
        success = success &&
            testValue("--Increment 0", --path == 0);
            
        BeeFishMisc::outputSuccess(success);
        
        return success;
    }

    
    inline bool testDeleteProperty()
    {

        cout << "Test delete property" << endl;
    
        bool success = true;
 
        JSONDatabase database;
        
        JSONPath start = database.host("https://test");
        
        JSONPathParser parser(start);
        parser.read("{\"a\":\"b\"}");
        parser.eof();
        
        success = success &
            testValue(
                "{\"a\":\"b\"}",
                parser.matched()
            );
            
        success = success &&
            testValue(
                "Words contain a",
                database.words().contains("a")
            );
            
        success = success &&
            testValue(
                "Words contain b",
                database.words().contains("b")
            );
        
        if (success)
        {
            start.deleteProperty("a");
            
            testValue(
                "Object property a removed",
                start.getChildren().isDeadEnd()
            );
            
            stringstream stream;
            stream << start;
            success = success &&
                testValue(
                    "Result {}",
                    stream.str() == "{}"
                );
            
        }
        
        
        success = success &&
            testValue(
                "Property a removed",
                !database.properties().contains("a")
            );
            
        success = success &&
            testValue(
                "Word b removed",
                !database.words().contains("b")
            );
            

        
        success = success &&
            testValue(
                "Word a removed",
                !database.words().contains("a")
            );
            
        assert(success);
        
        cout << endl;
        
        JSONDatabase database2;
        
        JSONPath start2 = database2.host("https://test");
        JSONPathParser parser2(start2);
        parser2.read("{\"a\":{\"b\":\"c\"}}");
        parser2.eof();
        
        
        success = success &
            testValue(
                "{\"a\":{\"b\":\"c\"}}",
                parser2.matched()
            );
            
        success = success &&
            testValue(
                "Words 2 contain a",
                database2.words().contains("a")
            );
            
        success = success &&
            testValue(
                "Words 2 contain b",
                database2.words().contains("b")
            );
            
        success = success &&
            testValue(
                "Words 2 contain c",
                database2.words().contains("c")
            );
        
        
        if (success)
        {
            cout << "Removing property b" << endl;
            
            success = success && testValue(
                "Property a is object",
                start2["a"].type() == Type::OBJECT
            );
            
            start2["a"].deleteProperty("b");
            success = success && testValue(
                "Property a is still object",
                start2["a"].type() == Type::OBJECT
            );
            
            success = success && testValue(
                "Property b removed",
                start2["a"].getChildren().isDeadEnd()
            );
            
            stringstream stream;
            stream << start2["a"];
            success = success &&
                testValue(
                    "Result", 
                    BString(stream.str()) == "{}"
                );
        }
            
        success = success &&
            testValue(
                "Property b removed",
                !database2.properties().contains("b")
            );
            
        success = success &&
            testValue(
                "Property c removed",
                !database2.words().contains("c")
            );
            
        assert(success);
        
        success = success &&
            testValue(
                "Words 2 still contain a",
                database2.words().contains("a")
            );
            
            

            
        JSONDatabase database3;
        JSONPath start3 = database3.host("https://test");
        JSONPathParser parser3(start3);
        parser3.read("{\"a\":{\"b\":\"c\"}}");
        parser3.eof();
        
        success = success &
            testValue(
                "{\"a\":{\"b\":\"c\"}}",
                parser3.matched()
            );
        
        cout << "Removing property a" << endl;
        
        start3.deleteProperty("a");
        
        success = success &&
            testValue(
                "Property 3 a removed",
                !database3.properties().contains("a")
            );
            
        success = success &&
            testValue(
                "Property 3 value b removed",
                !database3.words().contains("b")
            );
        
        success = success &&
            testValue(
                "Property 3 value c removed",
                !database3.words().contains("c")
            );
        
        {
            stringstream stream;
            stream << start3;
            success = success &&
                testValue(
                    "Result {}",
                    stream.str() == "{}"
                );
            cerr << "RESULT " << stream.str() << endl;
        }
            
        

        JSONDatabase database4;
        JSONPath start4 = database4.host("https://test");
        JSONPathParser parser4(start4);
        parser4.read("{\"a\":{\"a\":\"a\"}}");
        parser4.eof();
        
        success = success &
            testValue(
                "{\"a\":{\"a\":\"a\"}}",
                parser4.matched()
            );
    
        cout << "Removing property a" << endl;
        
        start4["a"].deleteProperty("a");
        
        success = success &&
            testValue(
                "Property a remains",
                database4.properties().contains("a")
            );
            
       
            /*
        start4.clear();
        stringstream stream4;
        stream4 << start4;
        
        success = success &&
            testValue(
                "Cleared value is undefined",
                stream4.str() == "undefined"
            );
        
        */
        BeeFishMisc::outputSuccess(success);
        
        return success;
    }
    
    inline bool testObjects()
    {
        cout << "Test objects" << endl;
        
        bool success = true;

    
        auto listObjects =
        [] (Path objects) {

            Iterable<Index> iterable(objects);
            Index count = 0;
            for (auto it = iterable.begin();
                 it != iterable.end();
                 ++it)
            {
                cerr << "Index " << *it << endl;
                ++count;
            }
            cerr << "Count " << count << endl;
        };
            
        auto test =
        [&success, listObjects](const BString& json, Index objectCount)
        {
            cout << "\t " << json << endl;
            
            JSONDatabase database;
            Path objects = database.objects();
            JSONPath start = database.host("http://test");
            
            JSONPathParser parser(start);
            parser.read(json);
            parser.eof();
        
            success = success &&
                testValue(
                    "Array objects count",
                    objects.count() == objectCount
                );
            
            if (success)
               start.setUndefined();
            
            success = success &&
                testValue(
                    "Cleared objects count",
                    objects.count() == 0
                );
                
            if (!success)
                listObjects(objects);
                
            return success;
        };
        
        success = success &&
           test("{\"a\":\"b\"}", 2);
        success = success &&
           test("{\"a\":{\"a\":\"a\"}}", 3);
        success = success &&
           test("[1,2,3]", 4);
        
            
        BeeFishMisc::outputSuccess(success);
    
        return success;
    }
    
    inline bool testMultiThreaded()
    {
        cout << "Test multi threaded" << endl;
        const Index SIZE = 1327976;

        auto test =
        [SIZE](std::filesystem::path file, bool readOnly, bool getSuccess = false)
        {
            
            static Index size = 0;
            static bool success = true;
            
            
            if (getSuccess) {
                bool result = success && size ==  SIZE;
                success = true;
                size = 0;
                return result;
            }
            
            cout << "\tThread " << std::this_thread::get_id() << endl;
            
            JSONDatabase db(file, readOnly);
            
            JSONPath path = db.host("https://test");
            ifstream input(TEST_DIRECTORY "/90-Sample.json");
            JSONPathParser parser(path, cout);
            parser.read(input);
            input.close();
            success = testValue(
                "Parsed file",
                parser.matched()
            );
            
            if (success) {
                if (size == 0)
                {
                    size = db.size();
                    cout << "Size a " << size << endl;
                    success = testValue(
                        "First size",
                        size == SIZE
                    );
                }
                else {
                    success = testValue(
                        "Subsequent size",
                        db.size() == SIZE
                    );
                    cout << "Size a " << size << endl;
                    cout << "Size b " << db.size() << endl;
                }
            }
            
            return success;
            
        };
        
        bool success = true;
        
        {
            cout <<"\tTesting read only " << flush;
            File tempFile;
        
            test(tempFile.filename(), false);
            test(tempFile.filename(), true);
            success = test("", true, true);
        
            outputSuccess(success);
        }
        
        if (success)
        {
            cout <<"\tStarting threads" << endl;
         
            File tempFile;
            std::thread threads[] =
            {
                std::thread(test, tempFile.filename(), false),
                std::thread(test, tempFile.filename(), false)
            };
            

            cout << "\tWaiting threads" << endl;
        
            for (auto& thread : threads) {
                thread.join();
            }
        
            cout << "Expected size " <<  SIZE << endl;

            success = test("", true, true);
    
            outputSuccess(success);
        }
        
        return success;
        
    }
    
    
}

#endif
