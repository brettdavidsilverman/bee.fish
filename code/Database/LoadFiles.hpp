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
    std::filesystem::path directory,
    std::filesystem::path path
);

void loadFiles(
    BeeFishAuthentication::Authentication& auth,
    JSONPath start,
    std::filesystem::path directory
)
{

    cout
            << "Loading directory "
            << directory
            << endl;
            
    loadFile(
            auth,
            start,
            directory,
            "/home/brettdavidsilverman/bee.fish/dev.bee.fish/art-small.jpg"
        );
        
    return;
            
    const std::vector<BString> ignoreFiles {
        "deaths.json"
    };

    // Create the iterator explicitly
    auto it = filesystem::recursive_directory_iterator(
        directory, 
        filesystem::directory_options::skip_permission_denied
    );
        
    auto end_it = filesystem::recursive_directory_iterator();
    
    
    while (it != end_it) {
        
        const std::filesystem::path
            path = it->path();
        const BString filename = path.filename();
        if (find(
                ignoreFiles.begin(),
                ignoreFiles.end(),
                filename
            ) != ignoreFiles.end())
        {
            ++it;
            continue;
        }
    

        loadFile(
            auth,
            start,
            directory,
            path
        );
        
        ++it;
    }
}

void loadFile(
    BeeFishAuthentication::Authentication& auth,
    JSONPath start,
    std::filesystem::path directory,
    std::filesystem::path path
)
{
    const std::vector<BString> ignoreDirectories {
        ".git",
        "build"
    };
    
    std::filesystem::path relativePath =
        std::filesystem::relative(
            path, 
            directory
        ); 

    BString relative = relativePath.string();

    const std::vector segments =
        relative.split('/');
        
    auto onword =
    [&auth](JSONPath& path, const BString& word)
    {
        cout 
            << path.toString(auth)
            << "#"
            << word 
            << endl;
    };
    
        
        
    for (const auto& segment : segments)
    {
        
        if (find(
                ignoreDirectories.begin(),
                ignoreDirectories.end(),
                segment
            ) != ignoreDirectories.end()
        )
        {
            return;
        }
        
        start = start[segment];
        
            
    }
    
    cout << start.toString(auth) << " " << start.index() << endl;
    
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

    Index pageIndex = 0;
    
    start.database()._onword = onword;

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

        // this may throw with an range_error
        start.setString(page, pageIndex++, true,  partWord);
        
    }

    start.endString(pageIndex, true, partWord);

    input.close();
    
    
}

}

#endif
