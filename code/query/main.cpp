#include <iostream>
#include <fstream>
#include "Query.hpp"
#include "Test.hpp"

int main(int argc, const char* argv[]) {
    
    using namespace BeeFishQuery;
    
                

        /*
    {
        JSONDatabase db(BEE_FISH_DATABASE_FILE);
        Path word = db.words()["json"];
       // word.clear();
        Iterable<Index> iterable(word);
        for (auto index : iterable)
        {
            cout << index << " " << flush << 
            JSONPath(db, index).toString() << endl;
        }
        cout << "finished" << endl;
        return 0;
     }
*/
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

    JSONDatabase database(DATABASE_FILENAME);
    
    Words words(database.words());

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
                    
            //cout << *(statement._expression) << endl;
            
            PathBase* path =
                statement._expression
                ->getPath(words);
                
            Iterable<Index> jsonMatches(*path);
            for (auto index : jsonMatches)
            {
                JSONPath path(database, index);
                BString string = path.toString();
                
                if (string.length())
                    cout << string << endl;
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