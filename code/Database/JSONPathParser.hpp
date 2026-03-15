#ifndef BEE_FISH_DATABASE_JSON_PATH_PARSER_HPP
#define BEE_FISH_DATABASE_JSON_PATH_PARSER_HPP
#include "../json/json.h"
#include "../json/json-parser.h"
#include "JSONDatabase.hpp"
#include "JSONPath.hpp"
#include "NullStream.hpp"

namespace BeeFishDatabase {
 
    using namespace BeeFishParser;
    using namespace BeeFishScript;
    using namespace BeeFishJSON;
     
    class JSONPathParser :
        public JSONParser
    {
    private:
        JSONPath _start;
        vector<JSONPath> _pathStack;
        vector<Index> _indexStack;
        vector<BString> _keyStack;
        vector<Type> _typeStack;
        ostream& _log;
        
    public:
        using JSONParser::read;

        JSONPathParser(JSONPath path, Match& match, ostream& log = cnull) :
            JSONParser(match),
            _start(path),
            _log(log)
        {
        }
          
        JSONPathParser(JSONPath path, ostream& log = cnull) :
            JSONParser(),
            _start(path),
            _log(log)
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
        
        JSONPath start() {
            return _start;
        }
        
     private:
         
        
        
        virtual void setVariable(JSONPath start, const Type type, const BString& value)
        {
            
            
            JSONPath path = start;
            start.lock();
          //ScopedPathLock lock(start);
           // start.setType(type);
            
   
            

            switch (type)
            {
                case Type::UNDEFINED:
                    
                    start.setUndefined();
                    // Dont log undefined
                    return;
                case Type::NULL_:
                    start.setNull();
                    break;
                case Type::BOOLEAN:
                    start.setBoolean(value);
                    break;
                case Type::INTEGER:
                    start.setInteger(value);
                    break;
                case Type::NUMBER:
                    start.setNumber(value);
                    break;
                case Type::STRING:
                    start.setString(value);
                    break;
                case Type::ARRAY:
                case Type::OBJECT:
                    start.setType(type);
                    break;
                default:
                    throw std::logic_error("JSONPathParser::setVariable");
            }
            
            if (&_log != &cnull)
                _log << start.toString() << endl;
             
            
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
                path = _start;

            }
            else
            {
                path = topPath();
                
                if (topType() == Type::OBJECT)
                    path = path[topKey()];
                else {
                    path = path[topIndex()];
                }
                
            }
            path.setType(type);
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
        
        virtual void onobjectvalue(BeeFishJSON::Object* object, BeeFishJSON::ObjectKey* key, BeeFishJSON::JSON* json)
        override
        {

            JSONPath path = topPath();
            
            path = path[key->value()];
            setVariable(path, json->type(), json->value());
            
            pop_back_key();

            JSONParser::onobjectvalue(object, key, json);

            
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

            if (_pathStack.size() == 0)
            {
                JSONPath start = _start;
                setVariable(
                    start,
                    json->type(),
                    json->value()
                );
            }
                
            
                
        }
                          
          
          
    };

     
     
}

#endif
