#include <iostream>
#include <fstream>
#include "Query.hpp"
#include "Test.hpp"

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
    
    JSONDatabase database(DATABASE_FILENAME);
    
    bool display = true;
    if (hasArg(argc, argv, "-count") >= 0)
    {
        display = false;
    }
   
    auto displayResults =
    [&database, display](Expression& expression)
    {
        Words words(database.words());
        Bounds bounds = database.objects();
        
        PathBase* path =
            expression
            .getPath(words, bounds);
        Index count = 0;
        
        Iterable<Index> jsonMatches(*path);
        for (auto index : jsonMatches)
        {
            if (display) {
                JSONPath path(database, index);
                BString string = path.toString();
                cout << string << endl;
            }
          ++count;
        }
        
        cout << "Total count: " << count << endl;
            
        delete path;
    };
    
    
    int queryArg = hasArg(argc, argv, "-query");
    if (queryArg >= 0 && argc > queryArg + 1)
    {
        string line;
        line = argv[queryArg + 1];
        Expression expression;
        stringstream stream(line);
        stream >> expression;
        displayResults(expression);
        return 0;
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
                    
            displayResults(*statement._expression);
            
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