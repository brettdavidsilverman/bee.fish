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
    
    int originArg =
    hasArg(argc, argv, "-origin");
    BString origin = ORIGIN;
#ifdef DEBUG
    {
        origin += ":8000";
    }
#endif

    if (originArg != -1 && argc > (originArg + 1))
    {
        origin = argv[originArg + 1];
    }
    
    BeeFishWeb::URL url(origin);
    BeeFishAuthentication::Authentication
        auth(url.origin(), database.filename());
    auth.logon("boo");
        
    JSONPath path = JSONPath::fromString(
        auth,
        database,
        url
    );
    
    cout << "Using origin " << origin << endl;
    
    
    BString line;
    auto displayResults =
    [&database, &line, &auth, &path](Statement& statement)
    {
        
        cout << "Querying " 
             << path.toString(auth) 
             << "?" 
             << line
             << endl;
    
        Index count = 0;
        
        BeeFishQuery::Iterable
            jsonMatches(
                auth,
                database,
                statement
            );
            
        auto it = jsonMatches.begin();

        for (;
             it != jsonMatches.end();
             ++it)
        {
            cout << *it << endl;
            
            if (++count == 10)
                break;
        }
        
        if (it != jsonMatches.end())
        {

            cout << path.toString(auth)
                 << "?q="
                 << line.encodeURI()
                 << "&next="
                 << to_string(it.index())
                 << endl;
        }
        
       // cout << "Total count: " << count << endl;
            
    };
    
    
    int queryArg = hasArg(argc, argv, "-query");
    if (queryArg >= 0 && argc > queryArg + 1)
    {
        
        line = argv[queryArg + 1];
        Statement statement(path, line, JSONPath::Sort::Id);
        displayResults(statement);
        return 0; 
    }
    

    do 
    {
        cout << "Statement: ";
        
        try {
            
            std::getline(std::cin, line);
            
            line = line.trim();
            
            if (line == "exit")
            {
                break;
            }
            
            BeeFishQuery::Statement
                statement(
                    path, 
                    line, 
                    JSONPath::Sort::Id
                );
                    
            displayResults(statement);
            
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