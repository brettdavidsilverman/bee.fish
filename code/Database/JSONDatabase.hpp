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
        Path _authentication;
        Path _properties;
        Path _words;
        Path _json;
        

    public:

        JSONDatabase(
            const BString& filePath = ""
        )
        : Database(filePath)
        {
     
    
            _root = *this;
        
            _properties = 
                _root[PROPERTIES];
                
            _words =
                _root[WORDS];
                
            _authentication =
                _root[AUTHENTICATION];
            
            _json =
                _root[JSON];
        
        
        }
        
        Path root() const
        {
            return _root;
        }
        
        Path authentication()
        {
            return _authentication;
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
        JSONPath json() const;
    
        
        // defined in JSONPath
        JSONPath host(const BString& host) const;
        

        
        
        
    };
    
}

#endif