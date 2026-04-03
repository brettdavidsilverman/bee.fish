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
        Path _objects;
        Path _parentChildren;
        Path _json;
        

    public:

        JSONDatabase(
            const BString& filePath = "",
            bool readOnly = false
        )
        : Database(filePath, readOnly)
        {

    
            _root = *this;
        
            _properties = 
                _root[PROPERTIES];
                
            _words =
                _root[WORDS];
                
            _objects =
                _root[OBJECTS];
                
            _parentChildren =
                _root[PARENT_CHILDREN];
                
            _authentication =
                _root[AUTHENTICATION];
            
            _json =
                _root[JSON];
        
        }

        virtual Index rootIndex() 
        override
        {
            return _json.index();
        }
        /*
        Path root()
        {
            return _root;
        }
        */

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
        
        Path objects() const
        {
            return _objects;
        }
        
        Path parentChildren() const
        {
            return _parentChildren;
        }
        
        Path userData(const BString& userId)
        {
            return _root[USERS][userId];
        }
        
        Path sessionData(
            const BString& ipAddress,
            const BString& sessionId)
        {
            return _root
                    [IP_ADDRESSES]
                    [ipAddress]
                    [sessionId];
                
        }
        
        // defined in JSONPath
        JSONPath json() const;
    
        
        // defined in JSONPath
        JSONPath host(const BString& host) const;
        

        
        
        
    };
    
}

#endif