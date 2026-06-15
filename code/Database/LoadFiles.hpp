#ifndef BEE_FISH_DATABASE_LOAD_FILES_HPP
#define BEE_FISH_DATABASE_LOAD_FILES_HPP

#include <filesystem>
#include "../Miscellaneous/MimeTypes.hpp"
#include "Path.hpp"

namespace BeeFishDatabase {
    
using namespace BeeFishMiscellaneous;

void loadFile(
    BeeFishAuthentication::Authentication& auth,
    JSONPath start,
    std::filesystem::path path
);

void loadFiles(
    BeeFishAuthentication::Authentication& auth,
    JSONPath start,
    const BString& directory
)
{

    cout
            << "Loading directory "
            << directory
            << endl;
            
    const std::vector<BString> ignoreDirectories {
        ".git",
        "build"
    };
    
    const std::vector<BString> ignoreFiles {
        "deaths.json"
    };

    // Create the iterator explicitly
    auto it = filesystem::recursive_directory_iterator(
        directory.str(), 
        filesystem::directory_options::skip_permission_denied
    );
        
    auto end_it = filesystem::recursive_directory_iterator();
    
    
    while (it != end_it) {
        
        const BString path = it->path();
        const BString filename = it->path().filename();
        if (find(
                ignoreFiles.begin(),
                ignoreFiles.end(),
                filename
            ) != ignoreFiles.end())
        {
            ++it;
            continue;
        }
        
        const BString relative =
            path.substr(
                directory.length() + 1
            );
            
        const std::vector segments =
            relative.split('/');

        JSONPath databasePath = start;
        bool skip = false;
        
        for (const auto& segment : segments)
        {
            const BString pathSegment = segment;
            if (pathSegment != "/")
            {
                if (find(
                        ignoreDirectories.begin(),
                        ignoreDirectories.end(),
                        pathSegment
                    ) != ignoreDirectories.end()
                )
                {
                    skip = true;
                    break;
                }
                
                databasePath = databasePath[
                    pathSegment
                ];
            }
            
        }
    
        if (!skip) {
            loadFile(auth, databasePath, path.str());
        }
        
        ++it;
    }
}

void loadFile(
    BeeFishAuthentication::Authentication& auth,
    JSONPath start,
    std::filesystem::path path
)
{
    BString extension = path.extension();
    if (!extension.length()) {
        extension = path.filename();
    }
    
    if (!_mimeTypes.count(
            extension
        ) ||
        !_mimeTypes[extension].index
    )
    {
        return;
    }
                
    cout << start.toString(auth) << endl;
    
    Index pageIndex = 0;
    
    start._onword =
    [&auth, &pageIndex](JSONPath& path, const BString& word)
    {
        cout 
            << path.toString(auth)
            << "#"
            << word 
            << endl;
    };
    

    File input(path.string(), true);
    
    Index pageSize = getPageSize();
    BString buffer(pageSize, '\0');

    Index fileSize = input.size();
    Index size = 0;
    Index total = 0;
    
    BString partWord;
    while (total < fileSize)
    {
        if (total + pageSize > fileSize)
            size = fileSize - total;
        else
            size = pageSize;
            
        input.read(buffer.data(), size);
        total += size;
        
        const BString page = buffer.substr(0, size);

        // this my throw with an range_error
        start.setString(page, pageIndex++, true,  partWord);
        
    }

    start.endString(pageIndex, true, partWord);

    input.close();
}

}

#endif
