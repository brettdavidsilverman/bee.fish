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
    std::filesystem::path path,
    ostream& log
);

void loadFiles(
    BeeFishAuthentication::Authentication& auth,
    JSONPath start,
    std::filesystem::path directory,
    ostream& log = cout
)
{

    cout
            << "Loading directory "
            << directory
            << endl;
/*
    cerr << "File art-small.jpg" << endl;
    loadFile(
            auth,
            start,
            directory,
            "/home/brettdavidsilverman/bee.fish/dev.bee.fish/art-small.jpg",
            cout
        );
    cerr << "Done" << endl;
    return;
*/

    const std::vector<BString> ignoreFiles {
        "deaths.json",
        "deaths-converted.json"
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
            path,
            log
        );
        
        ++it;
    }
}

void loadFile(
    BeeFishAuthentication::Authentication& auth,
    JSONPath start,
    std::filesystem::path directory,
    std::filesystem::path path,
    ostream& log
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
        
    auto onlog =
    [&auth, &log](JSONPath& path, const BString& word)
    {
        JSONDatabase::log(
            auth,
            log,
            path,
            word
        );
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
    
    
    if (std::filesystem::is_directory(path))
        return;
    
    BString extension = path.extension();
    if (!extension.length()) {
        extension = path.filename();
    }
    
                
    if (!_mimeTypes.count(
            extension
        )
    )
    {
        return;
    }
    
    bool index = _mimeTypes[extension].index;
    
    Index pageIndex = 0;
    
    start.database()._onlog = onlog;

    
    JSONPath http = start["{HTTP}"];
    BString contentType =
        _mimeTypes[extension].contentType;
        
    http["content-type"].setString(
        contentType
    );
    
    File input(path.string(), true);
                    
    http["content-length"].setInteger(input.size());
     
    JSONPath content = http["content"];
    
    Index pageSize = getPageSize();
    BString buffer(pageSize, '\0');

    Index fileSize = input.size();
    Index size = 0;
    Index total = 0;
    BString partWord;
    
    PagedStream pagedStream(
        [&content, &contentType, &pageIndex, &partWord]
        (const BString& encoded)
        {
            if (pageIndex == 0)
            {
                BString header = 
                    BString("data:") + 
                    contentType +
                    BString(";base64,") +
                    encoded;
                                    
                content.setString(
                    header,
                    pageIndex++, 
                    false, 
                    partWord);
            }
            else {
                content.setString(
                    encoded, 
                    pageIndex++,
                    false,
                    partWord
                );
            }
        }
    );
                 
    BeeFishMisc::Base64EncodeStream
         base64(pagedStream);
                        
    
    while (total < fileSize)
    {
        if (total + pageSize > fileSize)
            size = fileSize - total;
        else
            size = pageSize;
            
        input.read(buffer.data(), size);
        total += size;
        
        const BString page = buffer.substr(0, size);
        
        if (index)
        {
            // this may throw with an range_error
            content.setString(page, pageIndex++, index, partWord);
        }
        else
        {
            base64 << page;
        }
        
    }
    
    if (!index) {
        base64.flush();
        pagedStream.flush();
    }
    
    content.endString(pageIndex, index, partWord);
    input.close();
    
    log << start.toString(auth) << endl;
}

}

#endif
