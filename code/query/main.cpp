#include <iostream>
#include <fstream>
#include "query.hpp"
#include "test.hpp"

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
        cout << "Testing query " << endl << endl;
        /*
        if (!BeeFishQuery::testAndPath())
            return 1;
            */
        if (!BeeFishQuery::test())
            return 1;
                
        return 0;
    }
    
    
    string line;
    Database db;
    Words<Index> words(db);
    
    do 
    {
        cout << "Expression: ";
        
        try {
             
            Statement statement;
            cin >> statement;
        
            if (statement.value().trim() == "exit")
            {
                break;
            }
                    
            cout << *(statement._expression) << endl;
            
            JoinPathBase<Index>* path =
                statement._expression
                ->getPath<Index>(words);
                
            delete path;
            
            cout << "ok" << endl;
        
        }
        catch (runtime_error& error)
        {
            cout << "Error " << error.what() << endl;
        }
            
    }
    while (!cin.eof());
  
    for (auto word : words) {
        cout << word << endl;
    }
    
    cout << "Bye" << endl;
      
    return 0;
}