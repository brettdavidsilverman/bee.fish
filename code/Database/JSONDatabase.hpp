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
        Path _users;
        Path _properties;
        Path _words;
        Path _objects;
        Path _json;
        

    public:
        
        typedef std::function<void(JSONPath& path, const BString& word)> OnWord;
        OnWord _onword = nullptr;


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
                
            _users = 
                _root[USERS];
            
            _authentication =
                _root[AUTHENTICATION];
            
            _json =
                _root[JSON];
        
        }

        virtual Index rootIndex() const
        override
        {
            return _json.index();
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
        
        Path objects() const
        {
            return _objects;
        }
        
        Path users() const
        {
            return _users;
        }
        
        Path userData(const BString& userId)
        {
            return _users[userId];
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
        JSONPath origin(const BString& origin) const;
        
        
        
        
        
    };
    
}

#endif