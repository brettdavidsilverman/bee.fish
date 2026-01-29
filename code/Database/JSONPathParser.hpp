#ifndef BEE_FISH_DATABASE_JSON_PATH_PARSER_HPP
#define BEE_FISH_DATABASE_JSON_PATH_PARSER_HPP
#include "../json/json.h"
#include "../json/json-parser.h"
#include "JSONDatabase.hpp"
#include "JSONPath.hpp"


namespace BeeFishDatabase {
 
    using namespace BeeFishParser;
    using namespace BeeFishScript;
    using namespace BeeFishJSON;
     
    class JSONPathParser :
        public JSONParser,
        public JSONPath
    {
    protected:
          
        vector<JSONPath> _pathStack;
        vector<Index> _indexStack;
        vector<BString> _keyStack;
        vector<Type> _typeStack;
        
    public:
        using JSONParser::read;

        JSONPathParser(JSONPath path, Match& match) :
            JSONParser(match),
            JSONPath(path)
        {
        }
          
        JSONPathParser(JSONPath path) :
            JSONParser(),
            JSONPath(path)
        {
        }
          
        JSONPathParser(Path path) :
            JSONParser(),
            JSONPath(path)
        {
        }

        virtual ~JSONPathParser()
        {

#ifdef DEBUG
            assert(_pathStack.size() == 0);
            assert(_indexStack.size() == 0);
            assert(_keyStack.size() == 0);
            assert(_typeStack.size() == 0);
#endif

        }
        
     private:

        virtual void setVariable(JSONPath start, const Type type, const BString& value)
        {
            JSONPath::Id id = start.id();
            

            
            Path words = JSONPath::words();
            switch (type)
            {
                case Type::UNDEFINED:
                {
                    JSONPath parent = start.clear();
                    break;
                }
                case Type::NULL_:
                {
                    Path path = start[type];
                    cout << start.toString() << "/null" << endl;
            
                    break;
                }
                case Type::BOOLEAN:
                case Type::INTEGER:
                case Type::NUMBER:
                {
                    Path path = start[type];
                    path.setData(value);
                    cout << start.toString() << "/" << value << endl;
            
                    break;
                }
                case Type::STRING:
                {
                    Path path = start[type];
                    path.setData(value);
                    BString copy(value);
                    char* str = copy.data();
                    const char* delims =
                        " .,!?()/\r\n\t\"\'{}:";
                    char* token;

                    // Get the first token
                    token = strtok(str, delims);

                    // Loop through all remaining tokens
                    while (token != nullptr) 
                    {
                        BString lower = 
                            BString(token).toLower();

                        Path word = 
                            words[lower];
                            
                        
                        word[id];
                        
                        JSONPath parentPath = start;
                        std::vector<BString> keys;
        
                        keys.push_back(lower);
                        
                        while(!parentPath.isRoot())
                        {
                            BString key;
                            word[parentPath.id()];
                            
                            parentPath = parentPath.parent(key);
                            keys.push_back(key.toLower());
                        }
                        
                        std::reverse(keys.begin(), keys.end());
                        
                        BString fullPath;
                        for (auto key : keys)
                        {
                            fullPath += key;
                            fullPath += "/";
                        }
                        
                        fullPath.pop_back();
                        
                        cerr << fullPath
                             << endl;
                            
                        token = strtok(nullptr, delims); 

                    }

                    break;
                }
                
                case Type::ARRAY:
                    //push_back_container(true);
                    break;
                case Type::OBJECT:
                    //push_back_container(false);
                    break;
                    
                default:
                    throw std::logic_error("JSONPathParser::setVariable");
            }

            
        }

          
        JSONPath& topPath() {
            return 
                _pathStack[_pathStack.size() - 1];
        }
          
        BString& topKey() {
            return 
                _keyStack[_keyStack.size() - 1];
        }
        

        Index& topIndex() {
            Index& index = 
                _indexStack
                [_indexStack.size() - 1];

            return index;
        }
        
        Type& topType() {
            Type& type = 
                _typeStack
                [_typeStack.size() - 1];

            return type;
        }
          
        void push_back_path(Type type)
        {
            JSONPath path;
            
            if (_pathStack.size() == 0)
            {
                path = *this;

            }
            else
            {
                path = topPath();
                
                if (topType() == Type::OBJECT)
                    path = path[topKey()];
                else {
                    Index index = topIndex();
                    path = path[index];
                }
                
            }
            path[type];
            _pathStack.push_back(path);
            if (type == Type::ARRAY)
                _indexStack.push_back(1);
            _typeStack.push_back(type);
        }

        void pop_back_path()
        {
            _pathStack.pop_back();
            if (topType() == Type::ARRAY)
                _indexStack.pop_back();
            _typeStack.pop_back();
        }
        
        void push_back_key(BString& key)
        {
            _keyStack.push_back(key);
        }
        
        void pop_back_key()
        {
            _keyStack.pop_back();
        }
        

        virtual void onbeginobject(BeeFishJSON::JSON* match) 
        override {
            
        
            push_back_path(Type::OBJECT);
        
        }
          
        virtual void onobjectkey(ObjectKey* key)
        override
        {
            push_back_key(
                key->value()
            );
            
        }
        
        virtual void onobjectvalue(BeeFishJSON::Object* object, BeeFishJSON::ObjectKey* key, BeeFishJSON::JSON* value)
        override
        {

            JSONPath path = topPath();
            path = path[key->value()];

            setVariable(path, value->type(), value->value());
        
            pop_back_key();

            JSONParser::onobjectvalue(object, key, value);

            
        }
          
          
        virtual void onendobject(BeeFishJSON::JSON* match)
        override
        {
    
            pop_back_path();
    
        }

        virtual void onbeginarray(BeeFishJSON::JSON* match)
        override
        {
            
            push_back_path(Type::ARRAY);
    
                
        }

        virtual void onarrayvalue(BeeFishJSON::JSON* json)
        override
        {
            
            JSONPath path = topPath();
            Index& index = topIndex();
            path = path[index++];
            setVariable(path, json->type(), json->value());
            

        }

        virtual void onendarray(BeeFishJSON::JSON* match)
        override
        {
    
            pop_back_path();
         
        }

        
        virtual void onvalue(BeeFishJSON::JSON* json)
        override
        {

            if (_pathStack.size() == 0 )
            {
                setVariable(
                    *this,
                    json->type(),
                    json->value()
                );
            }
                
            
                
        }
                          
          
          
    };

     
     
}

#endif
