#ifndef BEE_FISH_PARSER__TEST
#define BEE_FISH_PARSER__TEST

#include <iostream>

#include "query.h"

#include "../test/test.h"

using namespace std;
using namespace BeeFishTest;

namespace BeeFishQuery {
    
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
            new Capture(new Blankspace()),
           "\t",
           true,
           "\t"
        );
        
        ok &= testMatchDelete(
           "Space",
            new Capture(new Blankspace()),
           " ",
           true,
           " "
        );
        
        ok &= testMatchDelete(
           "Return",
            new Capture(new Blankspace()),
           "\r",
           true,
           "\r"
        );
        
        ok &= testMatchDelete(
           "New line",
            new Capture(new Blankspace()),
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
            new Capture(new BeeFishQuery::Character()),
           "a",
           true,
           "a"
        );
        
        ok &= testMatchDelete(
           "Space",
            new Capture(new BeeFishQuery::Character()),
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
            "Test token +",
             new Capture(new BeeFishQuery::Token()),
            "+",
            false
        );
        
        ok &= testMatchDelete(
            "Test token and ",
             new Capture(new BeeFishQuery::Token()),
            "and ",
            false
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
            "Test token or ",
             new Capture(new BeeFishQuery::Token()),
            "or ",
            false
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
            "Test token not ",
             new Capture(new BeeFishQuery::Token()),
            "not ",
            false
        );
        
        ok &= testMatchDelete(
            "Test token notother",
             new Capture(new BeeFishQuery::Token()),
            "notother",
            true,
            "notother"
        );

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
        
        BeeFishMisc::outputSuccess(ok);
        
        return ok;
    }
    
    inline bool testExpressions()
    {
        cout << "Test expressions" << endl;
        bool ok = true;
        
        
        ok &= testMatchDelete(
            "Expression token1 + token2",
            new Capture(new Expression()),
            "token1 + token2",
            true,
            "token1 + token2"
        );
        
        ok &= testMatchDelete(
            "Expression token1+token2",
            new Capture(new Expression()),
            "token1+token2",
            true,
            "token1+token2"
        );
        return ok;
        
        ok &= testMatchDelete(
           "Expression (hello)",
            new Capture(new Expression()),
           "(hello)",
           true,
           "(hello)"
        );
        
        ok &= testMatchDelete(
           "Expression ((hello))",
            new Capture(new Expression()),
           "((hello))",
           true,
           "((hello))"
        );
        
        ok &= testMatchDelete(
           "Expression (-hello)",
            new Capture(new Expression()),
           "(-hello)",
           true,
           "(-hello)"
        );
        

        ok &= testMatchDelete(
           "Expression (-(hello))",
            new Capture(new Expression()),
           "(-(hello))",
           true,
           "(-(hello))"
        );
        
        
        ok &= testMatchDelete(
           "Expression (+hello+goodbye)",
            new Capture(new Expression()),
           "(+hello+goodbye)",
           true,
           "(+hello+goodbye)"
        );
        
        ok &= testMatchDelete(
           "Operator complex and",
            new Capture(new Expression()),
           "(hello + goodbye + world)",
           true,
           "(hello + goodbye + world)"
        );
        
        BeeFishMisc::outputSuccess(ok);
        
        return ok;
        
    }
    
}

#endif
