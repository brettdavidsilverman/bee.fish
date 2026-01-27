#ifndef BEE_FISH_DATABASE_JSON_PARSER_HPP
#define BEE_FISH_DATABASE_JSON_PARSER_HPP
#include "../Database/DatabaseBase.hpp"
#include "../json/json.h"
#include "../json/json-parser.h"
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
        vector<Path> _keyWordPathStack;
        vector<Type> _typeStack;
        BString _key;
        
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
            assert(_keyWordPathStack.size() == 0);
            assert(_typeStack.size() == 0);
#endif

        }
        
     private:

        virtual void setVariable(JSONPath start, const Type type, const BString& value)
        {
            
            JSONPath path = start[type];

            JSONPath::Id id = start.id();
            
            for (auto keyPath : _keyWordPathStack)
            {
                keyPath[id];
            }
            
                     
            switch (type)
            {
                case Type::UNDEFINED:
                    path.clear();
                    break;
                case Type::NULL_:
                case Type::BOOLEAN:
                case Type::INTEGER:
                case Type::NUMBER:
                    path.setData(value);
                    break;
                case Type::STRING:
                {
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
                            words()[lower];
                            
                        word[id];
                        
                        for (auto parentPath : _pathStack)
                        {
                            word[parentPath.id()];
                        }
                        
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
          
        Path& topKeyWordPath() {
            return 
                _keyWordPathStack[_keyWordPathStack.size() - 1];
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
                    path = path[_key];
                else {
                    Index index = topIndex();
                    path = path[index];
                }
                
            }
            path[type];
            _pathStack.push_back(path);
            _indexStack.push_back(1);
            _typeStack.push_back(type);
        }
        
        void pop_back_path()
        {
            _pathStack.pop_back();
            _indexStack.pop_back();
            _typeStack.pop_back();
        }
          
/*
        void push_back_container(bool isArrayContainer)
        {
            _containerStack.push_back(isArrayContainer);
            _indexStack.push_back(1);
            
        }
        
        void pop_back_container()
        {
            _indexStack.pop_back();
            _containerStack.pop_back();
        }
        
*/
        void push_back_key_word_path(Path word)
        {
            _keyWordPathStack.push_back(word);
        }
        
        void pop_back_key_word_path()
        {
            _keyWordPathStack.pop_back();
        }
        

        virtual void onbeginobject(JSON* match) 
        override {
            
            push_back_path(Type::OBJECT);
        
        }
          
        virtual void onobjectkey(ObjectKey* key)
        override
        {
            _key = key->value();
            
            push_back_key_word_path(
                words()[_key.toLower()]
            );
            
        }
        
        virtual void onobjectvalue(BeeFishJSON::Object* object, ObjectKey* key, JSON* value)
        override
        {
            _key = key->value();

            JSONPath path = topPath();
            path = path[_key];
            ++topIndex();
            setVariable(path, value->type(), value->value());
        
            pop_back_key_word_path();

            JSONParser::onobjectvalue(object, key, value);
            
            
        }
          
          
        virtual void onendobject(JSON* match)
        override
        {
    
            pop_back_path();
    
        }

        virtual void onbeginarray(JSON* match)
        override
        {
            
            push_back_path(Type::ARRAY);
    
                
        }

        virtual void onarrayvalue(JSON* json)
        override
        {
            
            JSONPath path = topPath();
            Index& index = topIndex();
            path = path[index++];
            setVariable(path, json->type(), json->value());
            

        }

        virtual void onendarray(JSON* match)
        override
        {
    
            pop_back_path();
         
        }

        
        virtual void onvalue(JSON* json)
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
