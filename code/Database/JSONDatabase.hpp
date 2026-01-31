#ifndef BEE_FISH_DATABASE__JSON_DATABASE_HPP
#define BEE_FISH_DATABASE__JSON_DATABASE_HPP

#include "Database.hpp"
#include "JSONIndex.hpp"

namespace BeeFishDatabase
{

    class JSONPath;
    
    class JSONDatabase : public Database
    {
    protected:
        
        Path _root;
        Path _properties;
        Path _words;
        Path _json;
        Path _objects;
        Path _authentication;

    public:
        // defined in JSONPath.hpp
        JSONDatabase(
            const BString& filePath = ""
        );
        
        Path root() const
        {
            return _root;
        }
        
        Path properties() const
        {
            return _properties;
        }
        
        Path words() const
        {
            return _words;
        }
        
        // defined in JSONPath
        JSONPath origin(const BString& origin) const;
        
        // defined in JSONPath
        JSONPath json() const;
    
        
        Path authentication()
        {
            return _authentication;
        }
        
        
        
    };
    
}

#endif