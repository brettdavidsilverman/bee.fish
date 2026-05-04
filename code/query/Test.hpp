#ifndef BEE_FISH_QUERY__TEST
#define BEE_FISH_QUERY__TEST

#include <iostream>

#include "Query.hpp"

#include "../test/test.h"


namespace BeeFishQuery {

    using namespace std;
    using namespace BeeFishTest;
    using namespace BeeFishQuery;

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
        
        ok &= testMatchDelete(
           "Empty",
            new Capture(new Blankspaces()),
           "",
           true,
           ""
        );
        
        class CharPlus: public Character
        {
        public:
            CharPlus() : Character('+')
            {
            }
            
        };
        
        class Test : public BeeFishParser::And
        {
        public:
            Test() : BeeFishParser::And(
                new Blankspaces(),
                new CharPlus()
                
            )
            {
            }
            
        };
        
        ok &= testMatchDelete(
            "Test 1",
            new Capture(
                new Test()
            ),
            "+",
            true,
            "+"
        );

        ok &= testMatchDelete(
            "Repeat + +",
            new Capture(
                new Repeat<Test>()
            ),
            "+ +",
            true,
            "+ +"
        );
        
        ok &= testMatchDelete(
            "Repeat 2",
            new Capture(
                new Repeat<Test>()
            ),
            "++",
            true,
            "++"
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
        
        ok &= testMatchDelete(
            "Test word and",
             new Capture(new BeeFishQuery::Word()),
            "and",
            false
        );
        
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


        ok &= testMatchDelete(
            "Test word or",
             new Capture(new BeeFishQuery::Word()),
            "or",
            false
        );

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

        ok &= testMatchDelete(
            "Test word not",
             new Capture(new BeeFishQuery::Word()),
            "not",
            false
        );

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
        JSONDatabase db;
        JSONPath root = db.host("https://test");
    
        auto testmatch =
        [&ok, root](const BString& query, const BString& expected = "") {
            
            if (!ok)
                return false;
                
            Expression* expression =
                new Expression(root);
                
            Capture* capture =
                new Capture(expression);
                
            ok = testMatch(
                query,
                capture,
                query,
                true,
                query
            );

            if (ok && expression->matched()) {
                if (expected.size())
                {
                    stringstream stream;
                    stream << *expression;
                    ok = ok &&
                        testValue(
                            expected,
                            expected == stream.str()
                        );
                    if (!ok)
                        cout << "Got [" << stream.str() << "]" << endl;

                
                    if (ok) {
                        cout << "\t" << expected ;
                        expression->getPath();
                        outputSuccess(true);
                    }
                }
                
            }

            delete capture;
            
            return ok;
        };
        
        testmatch("word", "word");
        testmatch("word1 and word2", "(word1 and word2)");
        testmatch("(word)", "(word)");
        testmatch("( word )", "(word)");
        testmatch(" ( word ) ", "(word)");
        testmatch("((word))", "((word))");
        testmatch("word1 word2", "(word1 and word2)");
        testmatch("not word", "not word");
        testmatch("not (word)", "not (word)");
        testmatch("not(word)", "not (word)");
        testmatch("(not(word))", "(not (word))");
        
        testmatch("word1 + word2", "(word1 and word2)");
        testmatch("word1+word2", "(word1 and word2)");
        testmatch("word1 and word2", "(word1 and word2)");
        testmatch("word1 | word2", "(word1 or word2)");
        testmatch("word1|word2", "(word1 or word2)");
        testmatch("word1 or word2", "(word1 or word2)");
        
        testmatch("not (word1 or word2)", "not ((word1 or word2))");
        testmatch("!(word1 or word2)", "not ((word1 or word2))");
        testmatch("(word1 or word2)", "((word1 or word2))");
        testmatch("(word1 and word2)", "((word1 and word2))");
        testmatch("word1 and not word2", "(word1 and not word2)");
        
        testmatch("word1 and (word2 or word3)", "(word1 and ((word2 or word3)))");
        testmatch("word1 and word2 or word3", "((word1 and word2) or word3)");
        testmatch("(word1 and word2) or word3", "(((word1 and word2)) or word3)");
        
        testmatch("(word1 and word2) or (word3 and word4)", "(((word1 and word2)) or ((word3 and word4)))");
        testmatch("(word1 or word2) and (word3 or word4)", "(((word1 or word2)) and ((word3 or word4)))");
        testmatch("((word1 or word2))", "(((word1 or word2)))");
        
        testmatch("(word1 and word2) and not word3", "(((word1 and word2)) and not word3)");
        testmatch("(word1 and word2) and not (word3 or word4)", "(((word1 and word2)) and not ((word3 or word4)))");
        
        testmatch("word1 and word2 and word3", "((word1 and word2) and word3)");
        testmatch("word1 and not word2 and word3", "((word1 and not word2) and word3)");
        
        testmatch("not not word", "word");
        
        testmatch("word1 and and word2", "(word1 and word2)");
        testmatch("word1 and or word2", "(word1 or word2)");
        
        testmatch("word1$", "word1");
        
        if (ok)
        {
            cout << "\tInvalid expression: " << flush;
            
            try {
                ok = false;
                Expression expression(root, "and and");
            }
            catch (std::exception& ex)
            {
                ok = true;
            }
            
            outputSuccess(ok);
        }
        
        BeeFishMisc::outputSuccess(ok);
        
        return ok;
        
    }
    
    inline bool testAndPath()
    {
        cout << "Test And Path" << endl;
        
        bool ok  = true;
        
        JSONDatabase db;
        Words words(db);
        words = words["words"];
        Path bounds = db;
        bounds = bounds["bounds"];
        JSONPath::Id ids[5];
        
#ifdef JSON_INDEX
        Index count = 0;
        for (auto& id : ids)
        {
            id = count++;
        }
        
#endif

        words["one"][ids[0]];
        words["one"][ids[1]];
        words["one"][ids[2]];
        words["one"][ids[3]];
        words["two"][ids[2]];
        words["two"][ids[3]];
        words["two"][ids[4]];
        
        for (auto id : ids)
            bounds[id];
            
        JSONDatabase dbReadOnly(db.filename(), true);
        
        words = dbReadOnly;
        words = words["words"];
        bounds = dbReadOnly;
        bounds = bounds["bounds"];
        
        Path one = words["one"];
        Path two = words["two"];
        
        AndPath andPath(
            new Path(one),
            new Path(two),
            new Path(bounds)
        );
        
        
        if (ok) 
        {
            JSONPath::Id min = 
                andPath.min<JSONPath::Id>();
            
            ok = ok &&
                testResult(
                    "AndPath Minimum", 
                    min == ids[2]
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
            vector<JSONPath::Id> array;
            BeeFishDatabase::Iterable<JSONPath::Id>
                iterable(andPath);
                
            for (auto value : iterable)
            {
                array.push_back(value);
            }
        
            ok = ok &&
                testResult("AndPath result size",
                    array.size() == 2
                );

            ok = ok &&
                testResult("AndPath first result",
                    array[0] == ids[2]
                );
                
            ok = ok &&
                testResult("AndPath second result",
                    array[1] == ids[3]
                    
                );
                
        }
        
        BeeFishMisc::outputSuccess(ok);

        return ok;
        
    }
    
        
    inline bool testOrPath()
    {
        cout << "Test Or Path" << endl;
        
        bool ok  = true;
        
        JSONDatabase db;
        Words words(db);
        words = words["words"];
        Path bounds = db;
        bounds = bounds["bounds"];
        
        JSONPath::Id ids[3];
        
#ifdef JSON_INDEX
        Index count = 0;
        for (auto& id : ids)
            id = ++count;
#endif
        words["one"][ids[0]];
        words["two"][ids[1]];
        words["three"][ids[2]];
        
        
        
        
        for (auto id : ids)
            bounds[id];
            
                
        JSONDatabase dbReadOnly(db.filename(), true);
        words = dbReadOnly;
        words = words["words"];
        bounds = dbReadOnly;
        bounds = bounds["bounds"];

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
            JSONPath::Id min = 
                orPath.min<JSONPath::Id>();

            ok = ok &&
                testResult(
                    "OrPath Minimum", 
                    min == ids[0]
                );
            assert(ok);
        }
        
        if (ok) 
        {
            JSONPath::Id  max =
                orPath.max<JSONPath::Id>();
            
            ok = ok &&
                testResult(
                    "OrPath Maximum", 
                    max == ids[2]
                );
        }
        
        assert(ok);
        
        if (ok) 
        {
            Stack stack;
            orPath.save();

            bool result = orPath.next(stack);
            JSONPath::Id  value;
            stack.reset();
            stack >> value;
            
            
            ok = ok &&
                testResult(
                    "OrPath next 1", 
                    result
                );
                
            ok = ok &&
                testResult(
                    "OrPath next 1 value", 
                    value == ids[0]
                );
                
                
            if (ok) {
                result = orPath.next(stack);
                stack.reset();
                ok = ok &&
                    testResult(
                        "OrPath next 2", 
                        result
                    );
                
             
                if (ok) {
                    stack >> value;
                    
                    ok = ok &&
                        testResult(
                            "OrPath next 2 value", 
                            value == ids[2]
                        );
                }
                    
            }
    
            if (ok) {
        
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
            vector<JSONPath::Id> array;
            BeeFishDatabase::Iterable<JSONPath::Id>
                iterable(orPath);

            for (auto value : iterable)
            {
                array.push_back(value);
            }
        
            ok = ok &&
                testResult("OrPath result size",
                    array.size() == 2
                );

            ok = ok &&
                testResult("OrPath first result",
                    array[0] == ids[0]
                );
                
            ok = ok &&
                testResult("OrPath second result",
                    array[1] == ids[2]
                );
                
                
        }
        
        
        BeeFishMisc::outputSuccess(ok);
        
        
        return ok;
    }
    
    inline bool testNotPath()
    {
        cout << "Test Not Path" << endl;
        
        bool ok  = true;
        
        JSONDatabase db;
        Path path = db;
        
        Words words = path["words"];
        Bounds bounds = path["bounds"];
        
        JSONPath::Id ids[3];
        
#ifdef JSON_INDEX
        Index count = 0;
        for (auto& id : ids)
            id = ++count;

            
#endif
        words["one"][ids[0]];
        words["one"][ids[2]];
        words["two"][ids[1]];
        words["three"][ids[2]];
        
        for (auto id : ids) {
            bounds[id];
        }
        
        
        JSONDatabase dbReadOnly(db.filename(), true);
        path = dbReadOnly;
        words = path["words"];
        bounds = path["bounds"];
        
        Path one = words["one"];
        Path two = words["two"];
        Path three = words["three"];
        
        NotPath notPath(
            new Path(one),
            new Path(bounds)
        );
        
        
        if (ok) 
        {
            
            JSONPath::Id min = 
                notPath.min<JSONPath::Id>();
            
            ok = ok &&
                testResult(
                    "NotPath Minimum", 
                    min == ids[1]
                );
        }
        
    
        
        if (ok) 
        {
            Stack stack;
            notPath.save();

            bool result = notPath.next(stack);
            stack.reset();
            JSONPath::Id  value;
            stack >> value;
            
            
            ok = ok &&
                testResult(
                    "NotPath next 1", 
                    result
                );
                
            ok = ok &&
                testResult(
                    "NotPath next 1 value", 
                    value == ids[1]
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
            vector<JSONPath::Id> array;
            BeeFishDatabase::Iterable<JSONPath::Id>
                iterable(notPath);


            for (auto value : iterable)
            {

                array.push_back(value);
            }
        
            ok = ok &&
                testResult("NotPath result size",
                    array.size() == 1
                );

            ok = ok &&
                testResult("NotPath first result",
                    array[0] == ids[1]
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
                vector<JSONPath::Id> array;
                BeeFishDatabase::Iterable<JSONPath::Id>
                    iterable(notNotPath);

                for (auto value : iterable)
                {
                    array.push_back(value);
                }
        
                ok = ok &&
                    testResult("Not NotPath result size",
                        array.size() == 2
                    );

                ok = ok &&
                    testResult("Not NotPath first result",
                        array[0] == ids[0]
                    );
                    
                ok = ok &&
                    testResult("Not NotPath secondresult",
                        array[1] == ids[2]
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
        [&db, &words, &bounds](BString input, JSONPath::Id* ids, vector<Index> check) {
            
            
            cout << input << flush;
            
            Expression expression(db, words, bounds);
            Parser parser(expression);
            parser.read(input);
            parser.eof();
        
            bool ok = expression.matched();
        
            if (ok)
            {
                std::vector<JSONPath::Id> values;
                
                PathBase* path =
                    expression
                    .getPath();
                
                BeeFishDatabase::Iterable<JSONPath::Id>
                    iterable(*path);
                    
                for (auto value : iterable)
                {
                    values.push_back(value);
                }
             
                if (ok)
                {
                    for (unsigned int i = 0; i < values.size() && i < check.size(); ++i)
                    {
                        if (values[i] != ids[check[i]])
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
        
        JSONPath::Id ids[5];
#ifdef JSON_INDEX
        Index count = 0;
        for (auto& id : ids)
            id = ++count;
#endif
        
        words["one"][ids[0]];
        words["one"][ids[1]];
        words["one"][ids[2]];
        words["one"][ids[3]];
        words["two"][ids[2]];
        words["two"][ids[3]];
        words["two"][ids[4]];
        
        for (auto& id : ids)
            bounds[id];
            
        
        ok = ok && test("one", ids, {0,1,2,3});
        ok = ok && test("two", ids, {2,3,4});
        ok = ok && test("one and one", ids, {0,1,2,3});
        ok = ok && test("one and two", ids, {2,3});
        ok = ok && test("two and two", ids, {2,3,4});
        ok = ok && test("one and three", ids, {});
        ok = ok && test("one or two", ids, {0,1,2,3,4});
        ok = ok && test("not two", ids, {0, 1});
        ok = ok && test("one and not two", ids, {0, 1});
        ok = ok && test("not not two", ids, {2,3,4});
        ok = ok && test("not (not two)", ids, {2,3,4});
        ok = ok && test("three", ids, {});
        
        assert(ok);
        
        return ok;
        
    }
    
    inline bool testQueryJSON()
    {
        cout << "Test Query JSON" << endl;
    
        bool ok  = true;
        filesystem::path json = TEST_DIRECTORY "/45-Object.json";
        
        JSONDatabase database;
        JSONPath root = database.host("https://test")[json.filename()];
        JSONPathParser parser(root);
        Path objects = database.objects()[root.id()];
        Path words = database.words();
            
        ifstream input(json);
        parser.read(input);
        parser.eof();
        ok = parser.matched();
        
        JSONDatabase readOnlyDatabase(database.filename(), true);
        auto test =
        [&readOnlyDatabase, &words, &objects, &ok](BString query, vector<BString> check) {
            
            cout << "\t" << query << flush;

            JSONDatabase& db = readOnlyDatabase;
            
            
        
            
            Expression expression(db, words, objects);
            if (ok)
            {
                Parser parser(expression);
                parser.read(query);
                parser.eof();
        
                ok = expression.matched();

            }
            std::vector<BString> values;
            if (ok)
            {
                cout << ": " << expression << "..." << flush;
                
                PathBase* pathBase =
                    expression
                    .getPath();
                
                BeeFishQuery::Iterable iterable(db, *pathBase);
                
                for (auto value : iterable)
                {
                    values.push_back(value);
                }
             
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
        
                "https://test/45-Object.json/a/1/1",
                "https://test/45-Object.json/a/1/2",
                "https://test/45-Object.json/a/2/1",
                "https://test/45-Object.json/a/2/2"
            }
        );
        ok = ok && test("a", 
            {
                "https://test/45-Object.json/a/1/1",
                "https://test/45-Object.json/a/1/2",
                "https://test/45-Object.json/a/2/1",
                "https://test/45-Object.json/a/2/2",
            }
        );
        ok = ok && test("b", 
            {
                "https://test/45-Object.json/a/1/1",
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
                "https://test/45-Object.json/a/1/1",
                "https://test/45-Object.json/a/2/1"
            }
        );
        
        ok = ok && test("b or c", 
            {
                "https://test/45-Object.json/a/1/1",
                "https://test/45-Object.json/a/1/2",
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
                "https://test/45-Object.json/a/1/2",
                "https://test/45-Object.json/a/2/2",
            }
        );
        
        
        ok = ok && test("(not b) or c", 
            {
                "https://test/45-Object.json/a/1/2",
                "https://test/45-Object.json/a/2/2",
            }
        );
        
        
        ok = ok && test("z and b or c", 
            {
                "https://test/45-Object.json/a/1/2",
                "https://test/45-Object.json/a/2/2"
            }
        );
        
        ok = ok && test("z and b and h or c", 
            {
                "https://test/45-Object.json/a/1/2",
                "https://test/45-Object.json/a/2/2"
            }
        );
        

        BeeFishMisc::outputSuccess(ok);
        
        return ok;
        
    }
    
}

#endif
