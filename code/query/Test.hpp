#ifndef BEE_FISH_QUERY__TEST
#define BEE_FISH_QUERY__TEST

#include <iostream>

#include "Query.hpp"

#include "../test/test.h"


namespace BeeFishQuery {

    using namespace std;
    using namespace BeeFishTest;

    bool testBlankspaces();
    bool testCharacters();
    bool testWords();
    bool testOperators();
    bool testExpressions();
    bool testAndPath();
    bool testOrPath();
    bool testQueryAndPath();
    bool testQueryJSON();
    bool testNotPath();
    
    inline bool test() 
    {
     
        bool ok = true;
        
        ok = ok && testBlankspaces();
        ok = ok && testCharacters();
        ok = ok && testWords();
        ok = ok && testOperators();
        ok = ok && testExpressions();
        ok = ok && testAndPath();
        ok = ok && testOrPath();
        ok = ok && testNotPath();
        ok = ok && testQueryAndPath();
        ok = ok && testQueryJSON();
        
        if (ok)
            cout << "SUCCESS";
        else
            cout << "FAIL";
            
        BeeFishMisc::outputSuccess(ok);
            
        cout << endl;
        
        return ok;
  
    }
    
    inline bool testBlankspaces()
    {
        cout << "Test Blankspaces" << endl;
        bool ok = true;
            
        ok &= testMatchDelete(
           "Tab",
            new Capture(new Blankspaces()),
           "\t",
           true,
           "\t"
        );
        
        ok &= testMatchDelete(
           "Space",
            new Capture(new Blankspaces()),
           " ",
           true,
           " "
        );
        
        ok &= testMatchDelete(
           "Return",
            new Capture(new Blankspaces()),
           "\r",
           true,
           "\r"
        );
        
        ok &= testMatchDelete(
           "New line",
            new Capture(new Blankspaces()),
           "\n",
           true,
           "\n"
        );
        
        ok &= testMatchDelete(
           "Mixture",
            new Capture(new Blankspaces()),
           " \t ",
           true,
           " \t "
        );
        
        ok &= testMatchDelete(
           "Spaces",
            new Capture(new Blankspaces()),
           "   ",
           true,
           "   "
        );
        
        class Test : public BeeFishParser::And
        {
        public:
            Test() : BeeFishParser::And(
                new Blankspaces(0),
                new BeeFishParser::Character("+")
                
            )
            {
            }
        };

        ok &= testMatchDelete(
            "Repeat",
            new Capture(
                new Repeat<Test>()
            ),
            "+ +",
            true,
            "+ +"
        );

        BeeFishMisc::outputSuccess(ok);
        
        return ok;
    }
    
    inline bool testCharacters()
    {
        cout << "Test Characters" << endl;
        bool ok = true;
            
        ok &= testMatchDelete(
           "Test a",
            new Capture(new WordCharacter()),
           "a",
           true,
           "a"
        );
        
        ok &= testMatchDelete(
           "Space",
            new Capture(new WordCharacter()),
           " ",
           false
        );


        BeeFishMisc::outputSuccess(ok);

        return ok;
    }
    
    inline bool testWords()
    {
        cout << "Test words" << endl;
        bool ok = true;
            
        ok &= testMatchDelete(
            "Test word",
             new Capture(new BeeFishQuery::Word()),
            "word",
            true,
            "word"
        );
        
        ok &= testMatchDelete(
            "Test small word 1",
             new Capture(new BeeFishQuery::Word()),
            "b",
            true,
            "b"
        );
        
        ok &= testMatchDelete(
            "Test small word 2",
             new Capture(new BeeFishQuery::Word()),
            "a",
            true,
            "a"
        );
        
        ok &= testMatchDelete(
            "Test word +",
             new Capture(new BeeFishQuery::Word()),
            "+",
            false
        );
        /*
        ok &= testMatchDelete(
            "Test word and",
             new Capture(new BeeFishQuery::Word()),
            " and",
            false
        );
        */
        ok &= testMatchDelete(
            "Test word andother",
             new Capture(new BeeFishQuery::Word()),
            "andother",
            true,
            "andother"
        );
    
        ok &= testMatchDelete(
            "Test word |",
             new Capture(new BeeFishQuery::Word()),
            "|",
            false
        );
/*
        ok &= testMatchDelete(
            "Test word or",
             new Capture(new BeeFishQuery::Word()),
            " or",
            false
        );
*/
        ok &= testMatchDelete(
            "Test word oranother",
             new Capture(new BeeFishQuery::Word()),
            "oranother",
            true,
            "oranother"
        );
        
        ok &= testMatchDelete(
            "Test word oranother",
             new Capture(new BeeFishQuery::Word()),
            "anotheror",
            true,
            "anotheror"
        );
        
        ok &= testMatchDelete(
            "Test word !",
             new Capture(new BeeFishQuery::Word()),
            "!",
            false
        );
/*
        ok &= testMatchDelete(
            "Test word not",
             new Capture(new BeeFishQuery::Word()),
            " not",
            false
        );
*/

        ok &= testMatchDelete(
            "Test word notother",
             new Capture(new BeeFishQuery::Word()),
            "notother",
            true,
            "notother"
        );
        
        
        ok &= testMatchDelete(
            "Empty",
             new Capture(new BeeFishQuery::Word()),
            " ",
            false
        );


        BeeFishMisc::outputSuccess(ok);

        return ok;
    }
    
    
    
    
    inline bool testOperators()
    {
        cout << "Test operators" << endl;
        bool ok = true;
            
        ok &= testMatchDelete(
           "Operator +",
            new Capture(new BeeFishQuery::And()),
           "+",
           true,
           "+"
        );

        ok &= testMatchDelete(
           "Operator |",
            new Capture(new BeeFishQuery::Or()),
           "|",
           true,
           "|"
        );
        
        
        ok &= testMatchDelete(
           "Operator !",
            new Capture(new BeeFishQuery::Not()),
           "!",
           true,
           "!"
        );
        
        ok &= testMatchDelete(
           "Operator and",
            new Capture(new BeeFishQuery::And()),
           "and",
           true,
           "and"
        );

        ok &= testMatchDelete(
           "Operator or",
            new Capture(new BeeFishQuery::Or()),
           "or",
           true,
           "or"
        );
        
        
        ok &= testMatchDelete(
           "Operator not",
            new Capture(new BeeFishQuery::Not()),
           "not",
           true,
           "not"
        );
        
        BeeFishMisc::outputSuccess(ok);
        
        return ok;
    }
    
    inline bool testExpressions()
    {
        cout << "Test expressions" << endl;
        bool ok = true;
        BString query;
        Database db;
        Path root = db;
        Words words(root["words"]);
        Words objects(root["objects"]);
        
        auto testmatch =
        [&ok, &words, &objects](const BString& query) {
            
            
            
            if (!ok)
                return false;
                
            Expression* expression =
                new Expression();
                
            Capture* capture =
                new Capture(expression);
                
            ok &= testMatch(
                query,
                capture,
                query,
                true,
                query
            );
            
            if (expression->matched()) {
                 PathBase* path =
                    expression->getPath(words, objects);
                
                delete path;
            }
            
            delete capture;
            
            return ok;
        };
        
        testmatch("word");
        testmatch("(word)");
        testmatch("( word )");
        testmatch(" ( word ) ");
        
        testmatch("not word");
        testmatch("not (word1)");
        testmatch("not(word1)");
        testmatch("(not(word1))");
        
        testmatch("word1 + word2");
        testmatch("word1+word2");
        testmatch("word1 and word2");
        testmatch("word1 | word2");
        testmatch("word1|word2");
        testmatch("word1 or word2");
        
        testmatch("not (word1 or word2)");
        testmatch("!(word1 or word2)");
        testmatch("(word1 or word2)");
        testmatch("(word1 and word2)");
        testmatch("word1 and not word2");
        
        testmatch("word1 and (word2 or word3)");
        
        testmatch("(word1 and word2) or word3");
        
        testmatch("(word1 and word2) or (word3 and word4)");
        testmatch("(word1 or word2) and (word3 or word4)");
        testmatch("((word1 or word2))");
        
        testmatch("(word1 and word2) and not word2");
        testmatch("(word1 and word2) and not (word3 or word4)");
        
        BeeFishMisc::outputSuccess(ok);
        
        return ok;
        
    }
    
    inline bool testAndPath()
    {
        cout << "Test And Path" << endl;
        
        bool ok  = true;
        
        Database db;
        Words words(db);
        Path bounds = db;
        bounds = bounds["bounds"];
        
        words["one"][0];
        words["one"][1];
        words["one"][2];
        words["one"][3];
        words["two"][2];
        words["two"][3];
        words["two"][4];
        
        bounds[0];
        bounds[1];
        bounds[2];
        bounds[3];
        bounds[4];
        
        Path one = words["one"];
        Path two = words["two"];
        
        AndPath andPath(
            new Path(one),
            new Path(two),
            new Path(bounds)
        );
        
        
        if (ok) 
        {
            int min = andPath.min<Index>();
            
            ok = ok &&
                testResult(
                    "AndPath Minimum", 
                    min == 2
                );
        }
        
        if (ok) 
        {
            Stack stack;
            andPath.save();
            bool result = andPath.goToMin(stack);
            
            ok = ok &&
                testResult(
                    "AndPath min with stack", 
                    result
                );
                
            andPath.restore();
        }
        
            
        
        if (ok)
        {
            vector<Index> array;

            for (auto value : andPath)
            {
                array.push_back(value);
            }
        
            ok = ok &&
                testResult("AndPath result size",
                    array.size() == 2
                );

            ok = ok &&
                testResult("AndPath first result",
                    array[0] == 2
                );
                
            ok = ok &&
                testResult("AndPath second result",
                    array[1] == 3
                );
                
        }
        
        BeeFishMisc::outputSuccess(ok);

        return ok;
        
    }
    
        
    inline bool testOrPath()
    {
        cout << "Test Or Path" << endl;
        
        bool ok  = true;
        
        Database db;
        Words words(db);
        Path bounds = db;
        words["one"][0];
        words["two"][1];
        words["three"][2];
        
        
        bounds = bounds["bounds"];
        
        bounds[0];
        bounds[1];
        bounds[2];
        
        Path one = words["one"];
        Path two = words["two"];
        Path three = words["three"];
        
        OrPath orPath(
            new Path(one),
            new Path(three),
            new Path(bounds)
        );
        
        
        if (ok) 
        {
            int min = orPath.min<Index>();
            
            ok = ok &&
                testResult(
                    "OrPath Minimum", 
                    min == 0
                );
        }
        
        if (ok) 
        {
            int max = orPath.max<Index>();
            
            ok = ok &&
                testResult(
                    "OrPath Maximum", 
                    max == 2
                );
        }
        
        assert(ok);
        
        if (ok) 
        {
            Stack stack;
            orPath.save();

            bool result = orPath.next(stack);
            Index value;
            stack >> value;
            stack.reset();
            
            ok = ok &&
                testResult(
                    "OrPath next 1", 
                    result
                );
                
            ok = ok &&
                testResult(
                    "OrPath next 1 value", 
                    value == 0
                );
                
                
            if (ok) {
                result = orPath.next(stack);

                ok = ok &&
                    testResult(
                        "OrPath next 2", 
                        result
                    );
                
             
                if (ok) {
                    stack >> value;
                    stack.reset();
                    ok = ok &&
                        testResult(
                            "OrPath next 2 value", 
                            value == 2
                        );
                }
                    
            }
    
            if (ok) {
                cout << stack << endl;
                result = orPath.next(stack);

                ok = ok &&
                    testResult(
                        "OrPath ended", 
                        !result
                    );
                    
            }
            
            orPath.restore();
        }
        
        if (ok)
        {
            vector<Index> array;

            for (auto value : orPath)
            {
                array.push_back(value);
            }
        
            ok = ok &&
                testResult("OrPath result size",
                    array.size() == 2
                );

            ok = ok &&
                testResult("OrPath first result",
                    array[0] == 0
                );
                
            ok = ok &&
                testResult("OrPath second result",
                    array[1] == 2
                );
                
                
        }
        
        
        BeeFishMisc::outputSuccess(ok);
        
        
        return ok;
    }
    
    inline bool testNotPath()
    {
        cout << "Test Not Path" << endl;
        
        bool ok  = true;
        
        Database db;
        Path path = db;
        
        Words words = path["words"];
        Bounds bounds = path["bounds"];
        
        words["one"][0];
        words["one"][2];
        words["two"][1];
        words["three"][2];
        
        bounds[0];
        bounds[1];
        bounds[2];
        
        Path one = words["one"];
        Path two = words["two"];
        Path three = words["three"];
        
        NotPath notPath(
            new Path(one),
            new Path(bounds)
        );
        
        
        if (ok) 
        {
            int min = notPath.min<Index>();
            
            ok = ok &&
                testResult(
                    "NotPath Minimum", 
                    min == 1
                );
        }
        
        if (ok) 
        {
            notPath.save();
            Stack stack;
            notPath.goToMax(stack);
            notPath.restore();

            int max = notPath.max<Index>();
            
            ok = ok &&
                testResult(
                    "NotPath Maximum", 
                    max == 1
                );
        }
        
        assert(ok);
        
        if (ok) 
        {
            Stack stack;
            notPath.save();

            bool result = notPath.next(stack);
            Index value;
            stack >> value;
            stack.reset();
            
            ok = ok &&
                testResult(
                    "NotPath next 1", 
                    result
                );
                
            ok = ok &&
                testResult(
                    "NotPath next 1 value", 
                    value == 1
                );

    
            if (ok) {
                result = notPath.next(stack);

                ok = ok &&
                    testResult(
                        "NotPath ended", 
                        !result
                    );
                    
            }
            
            notPath.restore();
        }
        
        if (ok)
        {
            vector<Index> array;

            for (auto value : notPath)
            {

                array.push_back(value);
            }
        
            ok = ok &&
                testResult("NotPath result size",
                    array.size() == 1
                );

            ok = ok &&
                testResult("NotPath first result",
                    array[0] == 1
                );

        }
        
        if (ok)
        {
            NotPath notNotPath(
                new NotPath(
                    new Path(one),
                    new Path(bounds)
                ),
                new Path(bounds)
            );
        
            if (ok)
            {
                vector<Index> array;

                for (auto value : notNotPath)
                {
                    array.push_back(value);
                }
        
                ok = ok &&
                    testResult("Not NotPath result size",
                        array.size() == 2
                    );

                ok = ok &&
                    testResult("Not NotPath first result",
                        array[0] == 0
                    );
                    
                ok = ok &&
                    testResult("Not NotPath secondresult",
                        array[1] == 2
                    );
                
            }
        }
        
        
        BeeFishMisc::outputSuccess(ok);

        return ok;
    }
    
    inline bool testQueryAndPath()
    {
        cout << "Test Query And Path" << endl;
        JSONDatabase db;
        Words words = db.words();
        Bounds bounds = db.objects();
        
        bool ok  = true;
        auto test =
        [&db, &words, &bounds](BString input, vector<JSONPath::Id> check) {
            
            
            cout << input << flush;
            
            Expression expression;
            Parser parser(expression);
            parser.read(input);
            parser.eof();
        
            bool ok = expression.matched();
        
            if (ok)
            {
                PathBase* pathBase =
                    expression
                    .getPath(words, bounds);
                
                Iterable<JSONPath::Id> iterable(*pathBase);
                std::vector<JSONPath::Id> values;
                for (auto index : iterable)
                {
                    values.push_back(index);
                }
             
                delete pathBase;
            
                
                
                if (ok)
                {
                    for (unsigned int i = 0; i < values.size() && i < check.size(); ++i)
                    {
                        if (values[i] != check[i])
                        {
                            ok = false;
                            break;
                        }
                    }
                    
                    ok = ok and ( values.size() == check.size());
                    
                    if (!ok)
                    {
                        cout << endl << "Expected" << endl;
                        
                        for (unsigned int i = 0; i < check.size(); ++i)
                        {
                            cout << "\t" << check[i] << endl;
                        }
                        
                        cout << "-----" << endl;
                        
                        cout << "Got" << endl;
                        
                        for (unsigned int i = 0; i < values.size(); ++i)
                        {
                            cout << "\t" << values[i] << endl;
                        }
                    
                    
                    }
                }
        
                 
            }
        
            BeeFishMisc::outputSuccess(ok);
        
            return ok;
        };
        
        words["one"][0];
        words["one"][1];
        words["one"][2];
        words["one"][3];
        words["two"][2];
        words["two"][3];
        words["two"][4];
        
        bounds[0];
        bounds[1];
        bounds[2];
        bounds[3];
        bounds[4];
        
        ok = ok && test("one", {0,1,2,3});
        ok = ok && test("two", {2,3,4});
        ok = ok && test("one and one", {0,1,2,3});
        ok = ok && test("one and two", {2,3});
        ok = ok && test("two and two", {2,3,4});
        ok = ok && test("one and three", {});
        ok = ok && test("one or two", {0,1,2,3,4});
        ok = ok && test("not two", {0, 1});
        ok = ok && test("one and not two", {0, 1});
        ok = ok && test("not not two", {2,3,4});
        ok = ok && test("three", {});
        
        assert(ok);
        
        return ok;
        
    }
    
    inline bool testQueryJSON()
    {
        cout << "Test Query JSON" << endl;
    
        bool ok  = true;
        filesystem::path json = TEST_DIRECTORY "/45-Object.json";
        
        JSONDatabase db;
        JSONPath root = db.host("https://test")[json.filename()];
        JSONPathParser parser(root);
        
        ifstream input(json);
        parser.read(input);
        parser.eof();
        ok = parser.matched();
        
        auto test =
        [&db, &root, &ok](BString query, vector<BString> check) {
            
            cout << "\t" << query << flush;
            
            
            Path words = db.words();
            Path objects = db.objects()[root][OBJECT_CHILDREN];
            
        
            
            Expression expression;
            if (ok)
            {
                Parser parser(expression);
                parser.read(query);
                parser.eof();
        
                ok = expression.matched();
                
                /*
                Iterable<Index> test(bounds);
                
                for (auto i : test)
                {
                    JSONPath path(db, i);
                    cout << path.toString() << endl;
                }
                assert(false);
                */
            }
            std::vector<BString> values;
            if (ok)
            {
                cout << ": " << expression << "..." << flush;
                
                PathBase* pathBase =
                    expression
                    .getPath(words, objects);
                
                Iterable<JSONPath::Id> iterable(*pathBase);
                
                for (auto id : iterable)
                {
                    JSONPath path(db, id);
                    values.push_back(path.toString());
                }
             
                delete pathBase;
            
            }
                
            if (ok)
            {
                for (unsigned int i = 0; 
                    i < values.size() &&
                    i < check.size();
                    ++i)
                {
                    if (values[i] != check[i])
                    {
                        ok = false;
                        break;
                    }
                }
            }
        
            ok = ok && (values.size() == check.size());
            if (!ok) {
      
                cout << "Expected:" << endl;
                for (unsigned int i = 0; 
                        i < check.size();
                        ++i)
                {
                        
                    cout << check[i] << endl;
                }
                cout << "------" << endl;
                cout << "Got:" << endl;
                for (unsigned int i = 0; 
                        i < values.size();
                        ++i)
                {
                    cout << values[i] << endl;
                }
            
            }
        
            BeeFishMisc::outputSuccess(ok);
        
            return ok;
        };
/*
{
   "a": [
      [
         "b",
         "c"
      ],
      [
         "b",
         "c"
      ]
   ]
}
*/
        ok = ok && test("boo", {});
        ok = ok && test("json",
            {
                "https://test/45-Object.json",
                "https://test/45-Object.json/a",
                "https://test/45-Object.json/a/1",
                "https://test/45-Object.json/a/1/1",
                "https://test/45-Object.json/a/1/2",
                "https://test/45-Object.json/a/2",
                "https://test/45-Object.json/a/2/1",
                "https://test/45-Object.json/a/2/2"
            }
        );
        ok = ok && test("a", 
            {
                "https://test/45-Object.json/a",
                "https://test/45-Object.json/a/1",
                "https://test/45-Object.json/a/1/1",
                "https://test/45-Object.json/a/1/2",
                "https://test/45-Object.json/a/2",
                "https://test/45-Object.json/a/2/1",
                "https://test/45-Object.json/a/2/2",
            }
        );
        ok = ok && test("b", 
            {
                "https://test/45-Object.json",
                "https://test/45-Object.json/a",
                "https://test/45-Object.json/a/1",
                "https://test/45-Object.json/a/1/1",
                "https://test/45-Object.json/a/2",
                "https://test/45-Object.json/a/2/1"
            }
        );
        
                
        ok = ok && test("not b", 
            {
                "https://test/45-Object.json/a/1/2",
                "https://test/45-Object.json/a/2/2"
            }
        );
        
        
        ok = ok && test("a and not b", 
            {
                "https://test/45-Object.json/a/1/2",
                "https://test/45-Object.json/a/2/2"
            }
        );
        
        
        

        ok = ok && test("not not b", 
            {
                "https://test/45-Object.json",
                "https://test/45-Object.json/a",
                "https://test/45-Object.json/a/1",
                "https://test/45-Object.json/a/1/1",
                "https://test/45-Object.json/a/2",
                "https://test/45-Object.json/a/2/1"
            }
        );
        
        ok = ok && test("b or c", 
            {
                "https://test/45-Object.json",
                "https://test/45-Object.json/a",
                "https://test/45-Object.json/a/1",
                "https://test/45-Object.json/a/1/1",
                "https://test/45-Object.json/a/1/2",
                "https://test/45-Object.json/a/2",
                "https://test/45-Object.json/a/2/1",
                "https://test/45-Object.json/a/2/2"
            }
        );
        
        ok = ok && test("not (b or c)", 
            {
            }
        );
        
        ok = ok && test("not b or c", 
            {
                "https://test/45-Object.json",
                "https://test/45-Object.json/a",
                "https://test/45-Object.json/a/1",
                "https://test/45-Object.json/a/1/2",
                "https://test/45-Object.json/a/2",
                "https://test/45-Object.json/a/2/2",
            }
        );
        
        ok = ok && test("(not b) or c", 
            {
                "https://test/45-Object.json",
                "https://test/45-Object.json/a",
                "https://test/45-Object.json/a/1",
                "https://test/45-Object.json/a/1/2",
                "https://test/45-Object.json/a/2",
                "https://test/45-Object.json/a/2/2",
            }
        );
        

        BeeFishMisc::outputSuccess(ok);
        
        return ok;
        
    }
    
}

#endif
