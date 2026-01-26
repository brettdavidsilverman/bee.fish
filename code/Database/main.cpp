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
    
    
    int originArg =
        hasArg(argc, argv, "-origin");
    BString origin = ORIGIN;
    if (originArg != -1 && argc > (originArg + 2))
    {
        origin = argv[originArg + 1];
        cout << origin << endl;
        assert(false);
    }
    
    JSONDatabase database(origin, DATABASE_FILENAME);
    Path root = database.objects();
    
    cout << database << endl;
    
    bool propertiesArg =
        (hasArg(argc, argv, "-properties") != -1);
    
    if (propertiesArg)
    {
        cout << "Properties" << endl;
        Path propertiesPath = database.properties();
        Iterable<BString> properties(propertiesPath);
        for (auto property : properties)
        {
            cout << property << endl;
        }
    }
    
    bool wordsArg =
        (hasArg(argc, argv, "-words") != -1);
    
    if (wordsArg)
    {
        cout << "Words" << endl;
        Path wordsPath = database.words();
        Iterable<BString> words(wordsPath);
        for (auto word : words)
        {
            cout << word << endl;
        }
    }
    
    bool loadDeaths =
        (hasArg(argc, argv, "-deaths") != -1);

    if (loadDeaths)
    {
        cout << "Input deaths.json" << endl;
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
    
    if (loadDeaths)
    {
        JSONPathParser json(root);
        cout << WWW_ROOT_DIRECTORY "/deaths.json" << endl;
        
        ifstream file(WWW_ROOT_DIRECTORY "/deaths.json");
        
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

    return 0;

}
