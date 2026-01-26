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
        vector<bool> _containerStack;
        vector<Index> _arrayIndexStack;
        vector<Path> _keyWordPathStack;
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
            assert(_containerStack.size() == 0);
            assert(_arrayIndexStack.size() == 0);
            assert(_keyWordPathStack.size() == 0);
#endif

        }
        
     protected:

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
                case Type::OBJECT:
                    break;
                    
                default:
                    throw std::logic_error("JSONPathParser::setVariable");
            }

                      
        }

          
    public:
      
          
        JSONPath& topPath() {
            return 
                _pathStack[_pathStack.size() - 1];
        }
          
        bool topContainer() {
            return
                _containerStack
                [_containerStack.size() - 1];
        }
          
        Path& topKeyWordPath() {
            return 
                _keyWordPathStack[_keyWordPathStack.size() - 1];
        }
        

        Size& topArrayIndex() {
            return 
                _arrayIndexStack
                [_arrayIndexStack.size() - 1];
        }
          
        void push_back_path(JSONPath path)
        {
            _pathStack.push_back(path);
        }
        
        void pop_back_path()
        {
            _pathStack.pop_back();

        }
          
          
        void push_back_container(bool isArrayContainer)
        {
            _containerStack.push_back(isArrayContainer);
            if (isArrayContainer)
                _arrayIndexStack.push_back(0);
            
        }
        
        void pop_back_container()
        {
            bool isArrayContainer = topContainer();
            if (isArrayContainer) {
                _arrayIndexStack.pop_back();
            }
        
            _containerStack.pop_back();
        }
        

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
            
            push_back_key_word_path(
                Path()
            );
                
            
            if (_containerStack.size() == 0)
            {
                push_back_container(false);
                push_back_path(*this);
            }
            else
            {
                JSONPath path = topPath();
                if (topContainer() == true)
                {
                    Size& index = topArrayIndex();
                    path = path[index];
                }
                else
                    path = path[_key];
                    
                push_back_container(false);
                push_back_path(path);
            }
            
                
        }
          
        virtual void onobjectkey(ObjectKey* key)
        override
        {
            _key = key->value();
            

            _keyWordPathStack[
                _keyWordPathStack.size() - 1
            ] = words()[_key.toLower()];
            
        }
        
        virtual void onobjectvalue(BeeFishJSON::Object* object, ObjectKey* key, JSON* value)
        override
        {
            _key = key->value();

            JSONPath path = topPath();
            path = path[_key];

            setVariable(path, value->type(), value->value());
        
            JSONParser::onobjectvalue(object, key, value);
            
            
        }
          
          
        virtual void onendobject(JSON* match)
        override
        {
        
            pop_back_key_word_path();
            
            pop_back_path();
            pop_back_container();
    
        }

        virtual void onbeginarray(JSON* match)
        override
        {
            
            if (_containerStack.size() == 0)
            {
                push_back_container(true);
                push_back_path(*this);
            }
            else
            {
                
                
                JSONPath path = topPath();
                if (topContainer() == true)
                {
                    Index& index = topArrayIndex();
                    path = path[index];
                }
                else
                    path = path[_key];
                    
                push_back_path(
                    path
                );
                push_back_container(true);
            }
    
                
        }

        virtual void onarrayvalue(JSON* json)
        override
        {
            
            JSONPath path = topPath();
            Size& index = topArrayIndex();
            path = path[index++];
            
            setVariable(path, json->type(), json->value());
            

        }

        virtual void onendarray(JSON* match)
        override
        {
    
            pop_back_path();
            pop_back_container();
            
         
        }

        
        virtual void onvalue(JSON* json)
        override
        {

            if (_containerStack.size() == 0 )
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
