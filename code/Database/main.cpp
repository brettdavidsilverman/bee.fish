#include <stdio.h>
#include <iostream>
#include <chrono>
#include <sstream>

#include "../Miscellaneous/Miscellaneous.hpp"
#include "../Miscellaneous/SigHandler.hpp"

//#define VERBOSE

#include "Database.hpp"
#include "Path.hpp"
#include "Test.hpp"

using namespace BeeFishDatabase;
using namespace BeeFishMisc;
using namespace std;

int main(int argc, const char* argv[])
{


    clog << "bee.fish.database"
              << endl
          << "C++ run time: "
              << __cplusplus
              << endl
          << "Version: "
              << DATABASE_VERSION
              << endl;

    int filenameArg =
        hasArg(argc, argv, "-filename");
    BString filename = DATABASE_FILENAME;

    if (filenameArg != -1 && argc > (filenameArg + 1))
    {
        filename = argv[filenameArg + 1];
    }
    
    clog << "Using database " << filename << endl;
    
    bool unlock =
        (hasArg(argc, argv, "-unlock") != -1);
        
    if (unlock)
    {
        clog << "Unlocking" << endl;
        LockFile::unlock(filename);
    
        return 0;
    }
    
    bool test =
        (hasArg(argc, argv, "-test") != -1);

    if (test)
    {
        if (!BeeFishDatabase::test())
            return 1;
        return 0;
    }
    
    JSONDatabase database(filename);
    
    bool originsArg  =
        (hasArg(argc, argv, "-origins") != -1);
    
    if (originsArg)
    {
        clog << "Origins " << endl;
        JSONPath origins = database.json();
        for (auto origin : origins)
        {
            cout << origin << endl;
        }
    }
    
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
        
    JSONPath path =
            database.origin(url.origin());
            
    vector<BString> paths = 
        url.paths();
    
    for (auto p : paths) {
        path = path[p];
    }
            
    clog << "Using origin " << url << endl;
    
    bool propertiesArg =
        (hasArg(argc, argv, "-properties") != -1);
    
    if (propertiesArg)
    {
        clog << "Properties" << endl;
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
        clog << "Words" << endl;
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
        clog << "Input deaths.json" << endl;
    }

    
    if (loadDeaths)
    {
        JSONPath inputPath =
            path["deaths"];
            
        JSONPathParser parser(auth, inputPath, clog);
        clog << "Loading deaths.json" << endl;
        
        ifstream file(WWW_ROOT_DIRECTORY "/deaths.json");
        
        parser.read(file);
    }

    bool input =
        (hasArg(argc, argv, "-input") != -1);
        
    if (input)
    {
        clog << "Input" << endl;
    }
    
    
    if (input)
    {
        
        JSONPathParser parser(auth, path, clog);
        parser.read(cin);
    }
    
    bool output =
        (hasArg(argc, argv, "-output") != -1);
        
    if (output)
    {
        clog << "Output" << endl;
    }
    
    if (output)
    {
        
        cout << path << endl;
    }
    

    return 0;

}
