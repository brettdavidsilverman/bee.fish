#ifndef BEE_FISH_QUERY__TEST
#define BEE_FISH_QUERY__TEST

#include <iostream>

#include "query.hpp"

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
    bool testQueryAndPath();
    
    inline bool test() 
    {
     
        bool ok = true;
        
        ok = ok && testBlankspaces();
        ok = ok && testCharacters();
        ok = ok && testWords();
        ok = ok && testOperators();
        ok = ok && testExpressions();
        ok = ok && testAndPath();
        ok = ok && testQueryAndPath();
        
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
            "Test word -",
             new Capture(new BeeFishQuery::Word()),
            "-",
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
           "Operator -",
            new Capture(new BeeFishQuery::Not()),
           "-",
           true,
           "-"
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
        Words words(db);
        auto testmatch =
        [&ok, &words](const BString& query) {
            
            
            
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
                    expression->getPath(words);
                
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
        
        words["one"][0];
        words["one"][1];
        words["one"][2];
        words["one"][3];
        words["two"][2];
        words["two"][3];
        words["two"][4];
        
        Path one = words["one"];
        Path two = words["two"];
        
        AndPath andPath(
            new Iterable<Index>(one),
            new Iterable<Index>(two)
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
            Iterable<Index> iterable(andPath);
            
            for (auto value : iterable)
            {
                cerr << "Value: " << value << endl;
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
    
    inline bool testQueryAndPath()
    {
        cout << "Test Query And Path" << endl;
        JSONDatabase db("http://test");
        Words words(db.properties());
        
        words["one"][0];
        words["one"][1];
        words["one"][2];
        words["one"][3];
        words["two"][2];
        words["two"][3];
        words["two"][4];
            
        bool ok  = true;
        auto test =
        [&db, &words](BString input, vector<int> check) {
            
            
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
                    .getPath(words);
                
                Iterable<int> iterable(*pathBase);
                std::vector<int> values;
                for (auto index : iterable)
                {
                    values.push_back(index);
                }
             
                delete pathBase;
            
                ok = ( values.size() == check.size());
                
                if (ok)
                {
                    for (unsigned int i = 0; i < values.size(); ++i)
                    {
                        if (values[i] != check[i])
                        {
                            ok = false;
                            break;
                        }
                    }
                }
        
                 
            }
        
            BeeFishMisc::outputSuccess(ok);
        
            return ok;
        };
        
        ok = ok && test("one", {0,1,2,3});
        ok = ok && test("two", {2,3,4});
        ok = ok && test("one and one", {0,1,2,3});
        ok = ok && test("one and two", {2,3});
        ok = ok && test("two and two", {2,3,4});
        ok = ok && test("one and three", {});
        ok = ok && test("one and three", {});
        ok = ok && test("three", {});
        
        return ok;
        
    }
    
}

#endif
