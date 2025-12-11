#include <iostream>
#include <fstream>
#include "query.h"
#include "test.h"

int main(int argc, const char* argv[]) {
    
    using namespace BeeFishQuery;
    
    cout << "bee.fish.query"
              << endl
          << "C++ run time: "
              << __cplusplus
              << endl
          << "Version: "
              << BEE_FISH_QUERY_VERSION
              << endl;

    if (hasArg(argc, argv, "-test") >= 0)
    {
        cout << "Testing query..." << endl << endl;
        if (!BeeFishQuery::test())
            return 1;
                
        return 0;
    }
    
    
    string line;
    do 
    {
        cout << "Expression: " << flush;
        
        try {
             Expression expression;
       // Capture capture(expression);
        
            cin >> expression;
        
            if (expression.matched())
                cout << "Ok" << endl;
            else
                break;
        }
        catch (runtime_error& error)
        {
            cout << "Error " << error.what() << endl;
        }
            
    }
    while (!cin.eof());
  
    cout << "Bye" << endl;
      
    return 0;
}