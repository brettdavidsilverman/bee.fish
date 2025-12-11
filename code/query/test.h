#ifndef BEE_FISH_QUERY__TEST
#define BEE_FISH_QUERY__TEST

#include <iostream>

#include "query.h"

#include "../test/test.h"


namespace BeeFishQuery {

    using namespace std;
    using namespace BeeFishTest;

    bool testBlankspaces();
    bool testCharacters();
    bool testTokens();
    bool testOperators();
    bool testExpressions();
    
    inline bool test() 
    {
     
        bool ok = true;
        
        ok &= testBlankspaces();
        ok &= testCharacters();
        ok &= testTokens();
        ok &= testOperators();
        ok &= testExpressions();
        
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
            new Capture(new TokenCharacter()),
           "a",
           true,
           "a"
        );
        
        ok &= testMatchDelete(
           "Space",
            new Capture(new TokenCharacter()),
           " ",
           false
        );


        BeeFishMisc::outputSuccess(ok);

        return ok;
    }
    
    inline bool testTokens()
    {
        cout << "Test tokens" << endl;
        bool ok = true;
            
        ok &= testMatchDelete(
            "Test token",
             new Capture(new BeeFishQuery::Token()),
            "token",
            true,
            "token"
        );
        
        ok &= testMatchDelete(
            "Test small token 1",
             new Capture(new BeeFishQuery::Token()),
            "b",
            true,
            "b"
        );
        
        ok &= testMatchDelete(
            "Test small token 2",
             new Capture(new BeeFishQuery::Token()),
            "a",
            true,
            "a"
        );
        
        ok &= testMatchDelete(
            "Test token +",
             new Capture(new BeeFishQuery::Token()),
            "+",
            false
        );
        
        ok &= testMatchDelete(
            "Test token and",
             new Capture(new BeeFishQuery::Token()),
            "and",
            true,
            "and"
        );
        
        ok &= testMatchDelete(
            "Test token andother",
             new Capture(new BeeFishQuery::Token()),
            "andother",
            true,
            "andother"
        );
    
        ok &= testMatchDelete(
            "Test token |",
             new Capture(new BeeFishQuery::Token()),
            "|",
            false
        );
        
        ok &= testMatchDelete(
            "Test token or",
             new Capture(new BeeFishQuery::Token()),
            "or",
            true,
            "or"
        );
        
        ok &= testMatchDelete(
            "Test token oranother",
             new Capture(new BeeFishQuery::Token()),
            "oranother",
            true,
            "oranother"
        );
        
        ok &= testMatchDelete(
            "Test token -",
             new Capture(new BeeFishQuery::Token()),
            "-",
            false
        );
        
        ok &= testMatchDelete(
            "Test token not",
             new Capture(new BeeFishQuery::Token()),
            "not",
            true,
            "not"
        );
        
        ok &= testMatchDelete(
            "Test token notother",
             new Capture(new BeeFishQuery::Token()),
            "notother",
            true,
            "notother"
        );
        /*
        ok &= testMatchDelete(
            "Test token not{ }",
             new Capture(new BeeFishQuery::Token()),
            "not ",
            false
        );
        */
        
        ok &= testMatchDelete(
            "Empty",
             new Capture(new BeeFishQuery::Token()),
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
        
        auto testmatch =
        [&ok](const BString& query) {
            
            if (!ok)
                return false;
            
            ok &= testMatchDelete(
                query,
                new Capture(new Expression()),
                query,
                true,
                query
            );
            return ok;
        };
        
        
        testmatch("token1 + token2");
        testmatch("token1+token2");
        testmatch("token1 and token2");
        testmatch("token1 | token2");
        testmatch("token1|token2");
        testmatch("token1 or token2");
        testmatch("not token");
        
        testmatch("(token)");
        testmatch("( token )");
        testmatch(" ( token ) ");
        testmatch("not (token1)");
        testmatch("not(token1)");
        testmatch("not (token1 or token2)");

        testmatch("(token1 or token2)");
        testmatch("(token1 and token2)");
        testmatch("token1 and not token2");
        /*
        testmatch("(token1 and token2) and not token2");
        testmatch("(token1 and token2) and not (token2 or token3)");
        */
        testmatch("token1 and (token2 or token3)");
        testmatch("(token1 and token2) or token3");
        testmatch("(token1 and token2) or (token3 and token4)");
        testmatch("(token1 or token2) and (token3 or token4)");
        testmatch("((token1 or token2))");
        BeeFishMisc::outputSuccess(ok);
        
        return ok;
        
    }
    
}

#endif
