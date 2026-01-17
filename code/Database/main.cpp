#include <stdio.h>
#include <iostream>
#include <chrono>
#include <sstream>

#include "../Miscellaneous/Miscellaneous.hpp"
#include "../Miscellaneous/SigHandler.hpp"

#include "Database.hpp"
#include "Path.hpp"
#include "Test.hpp"

using namespace BeeFishDatabase;
using namespace BeeFishMisc;
using namespace std;

int main(int argc, const char* argv[])
{ 
    
    cout << "bee.fish.database"
              << endl
          << "C++ run time: "
              << __cplusplus
              << endl
          << "Version: "
              << DATABASE_VERSION
              << endl;

    bool test =
        (hasArg(argc, argv, "-test") != -1);

    if (test)
    {
        if (!BeeFishDatabase::test())
            return 1;
    }
    
    bool unlock =
        (hasArg(argc, argv, "-unlock") != -1);

    
    if (unlock)
    {
        cout << "Unlocking " << DATABASE_FILENAME << endl;
        Database db(DATABASE_FILENAME);
        db.unlockAll();
        return 0;
    }
    
    JSONDatabase database("test");
    
    cout << database << endl;
    
    Path root(database.origin());
    
    bool read =
        (hasArg(argc, argv, "-read") != -1);
    
    if (read)
    {
        cout << "Read" << endl;
    }
    
    bool large =
        (hasArg(argc, argv, "-large") != -1);

    if (large)
    {
        cout << "Input large.json" << endl;
    }

    bool input =
        (hasArg(argc, argv, "-input") != -1);
        
    if (input)
    {
        cout << "Input" << endl;
    }
    
    bool output =
        (hasArg(argc, argv, "-output") != -1);
        
    if (output)
    {
        cout << "Output" << endl;
    }
    
    if (large)
    {
        JSONPathParser json(root);
        cout << WWW_ROOT_DIRECTORY "/large.json" << endl;
        
        ifstream file(WWW_ROOT_DIRECTORY "/large.json");
        
        json.read(file);
    }

    if (input)
    {
        JSONPathParser json(root);
        json.read(cin);
    }
    
    if (output)
    {
        JSONPath path(root);
        cout << path << endl;
    }

    cout << database << endl;
    cout << "large.json 127411713" << endl;

    return 0;

}
