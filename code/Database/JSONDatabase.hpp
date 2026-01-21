#ifndef BEE_FISH_DATABASE__JSON_DATABASE_HPP
#define BEE_FISH_DATABASE__JSON_DATABASE_HPP

#include "Database.hpp"
#include "JSONIndex.hpp"

namespace BeeFishDatabase
{

    class JSONDatabase : public Database
    {
    protected:
        BString _origin;
        Path _root;
        Path _properties;
        Path _words;
        Path _objects;
        
    public:
        JSONDatabase(
            const BString& origin,
            const BString& filePath = ""
        )
            : Database(filePath),
            _origin(origin)
        {
     
            _root = Path(*this)[ORIGINS][_origin];
            
            _properties = 
                _root[PROPERTIES];
                
            _words =
                _root[WORDS];
                
            _objects =
                _root[OBJECTS];
        }
        
        
        Path properties() const
        {
            return _properties;
        }
        
        Path words() const
        {
            return _words;
        }
        
        
        virtual Path objects() const
        {
            return _objects;
        }
        
        
    };
    
}

#endif