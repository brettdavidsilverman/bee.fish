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

    // Create the iterator explicitly
    auto it = filesystem::recursive_directory_iterator(
        directory.str(), 
        filesystem::directory_options::skip_permission_denied
    );
        
    auto end_it = filesystem::recursive_directory_iterator();
    
    
    while (it != end_it) {
        
        const BString path = it->path();
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
            loadFile(auth, databasePath, it->path());
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
    
    File input(path);
    
    Index pageSize = getPageSize();
    BString buffer('\0', pageSize);
    Index size = 0;
    Index total = 0;
    Index pageIndex = 0;
    BString partWord;
    while (total < input.size())
    {
        if (total + pageSize > input.size())
            size = input.size() - total;
        else
            size = pageSize;
            
        input.read(buffer.data(), size);
        total += size;
        
        const BString page = buffer.substr(0, size);
        
        bool finalPart = (total == input.size());
        
        start.setString(page, ++pageIndex, true, finalPart, partWord);
        
    }

    assert(total == input.size());
    
    input.close();
}

}

#endif
