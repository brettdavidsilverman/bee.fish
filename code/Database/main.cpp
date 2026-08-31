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
#include "LoadFiles.hpp"

using namespace BeeFishDatabase;
using namespace BeeFishMisc;
using namespace std;

int main(int argc, const char* argv[])
{
    /*
    BeeFishId::Timestamp timestamp;
    
    cerr << timestamp << endl;
    
    auto worker =
    [](std::string filename)
    {
        Database* dbs[100];
        for (int i = 0; i < 100; ++i)
        {
            cerr << i << endl;
            Database* db = new Database(filename);
            dbs[i] = db;
        }
    
        for (int i = 0; i < 100; ++i)
        {
             cerr << i << endl;
             delete dbs[i];
        }
    };
    
    Database db;
    
    std::thread threads[] =
    {
        std::thread(worker, db.filename()),
        std::thread(worker, db.filename())
    };
    
    for (auto &thread : threads)
    {
        thread.join();
    }
    
    cerr << "Ok" << endl;
    
    return 0;
*/
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
    
    bool reset =
        (hasArg(argc, argv, "-reset") != -1);
        
    if (reset)
    {
        clog << "Resetting..." << flush;
        LockFile::reset(
            std::filesystem::path(
                filename.str()
            )
        );
        BeeFishId::Timestamp::Memory
            ::reset();
        clog << "Reset" << endl;
    }
    
    clog << "Opening " << filename << endl;
    JSONDatabase database(filename);
    
    bool lock =
        (hasArg(argc, argv, "-lock") != -1);
        
    
    if (lock)
    {
        
        Path path(database);
        clog << "Locking" << endl;
        Path::ScopedLock lock(path);
        std::this_thread::sleep_for(std::chrono::milliseconds(10000));
        clog << "Unlocking" << endl;
    }
    
    bool test =
        (hasArg(argc, argv, "-test") != -1);

    if (test)
    {
        if (!BeeFishDatabase::test())
            return 1;
        return 0;
    }
    
    
    
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

    if (originArg != -1 && 
        argc > (originArg + 1) &&
        !BString(argv[originArg + 1]).startsWith("-")
    )
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
    
    bool topWordsArg =
        (hasArg(argc, argv, "-top") != -1);
    
    if (topWordsArg)
    {
        clog << "Top Words: Sorting..." << endl;
        Database temp;
        Path sort(temp);
        
        Path wordsPath = database.words();
        Iterable<BString> words(wordsPath);
        for (auto word : words)
        {
            Index count;
            
            wordsPath[word].getData<Index>(count);
            
            sort[count][word];
        }
        
        Index count = 0;
        Iterable<Index> top(sort);
        for (auto it = top.rbegin(); 
             it != top.rend() && count < 100;
             ++it)
        {
            Iterable<BString> words(sort[*it]);
            for (auto word : words)
            {
                cout << ++count << "\t" << word << "\t" << *it << endl;
            }
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
            
        JSONPathParser parser(auth, inputPath, cout);
        clog << "Loading deaths.json" << endl;
        
        ifstream file(WWW_ROOT_DIRECTORY "/deaths.json");
        
        parser.read(file);
    }
    
    BString loadDirectory = WWW_ROOT_DIRECTORY;
    int loadFilesArg =
        hasArg(argc, argv, "-files");
        
    if (loadFilesArg != -1 && 
        argc > (loadFilesArg + 1) &&
        !BString(argv[loadFilesArg + 1]).startsWith("-")
    )
    {
        loadDirectory = argv[loadFilesArg + 1];
    }
    
    bool loadFiles = loadFilesArg != -1;
    
    if (loadFiles) {
        BeeFishDatabase::loadFiles(
            auth,
            path,
            std::filesystem::path(loadDirectory.str())
        );
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
        path.write(auth, cout);
    }
    

    return 0;

}
