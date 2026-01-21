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

    JSONDatabase database(ORIGIN, DATABASE_FILENAME);
    JSONPath root = database.root();

    Words words(database.properties());

    {
        Iterable<BString> iterable(words);
        for (auto word : iterable)
        {
            cout << word << endl;
        }
    }
        
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
            
            PathBase* path =
                statement._expression
                ->getPath(words);
                
            Iterable<Index> jsonMatches(*path);
            for (auto index : jsonMatches)
            {
                JSONPath path(database, index);
                cout << path << endl;
            }
            
            delete path;
            
            cout << "ok" << endl;
        
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