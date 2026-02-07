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
        public JSONParser,
        public JSONPath
    {
    protected:
          
        vector<JSONPath> _pathStack;
        vector<Index> _indexStack;
        vector<BString> _keyStack;
        vector<Type> _typeStack;
        ostream& _log;
        
    public:
        using JSONParser::read;

        JSONPathParser(JSONPath path, Match& match, ostream& log = cnull) :
            JSONParser(match),
            JSONPath(path),
            _log(log)
        {
        }
          
        JSONPathParser(JSONPath path, ostream& log = cnull) :
            JSONParser(),
            JSONPath(path),
            _log(log)
        {
        }
          
        JSONPathParser(Path path, ostream& log = cnull) :
            JSONParser(),
            JSONPath(path),
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
        
     private:

        void addWord(Path words, const BString& word, JSONPath path)
        {
            static const char* _whiteSpace =
                " \r\n\v\t";
                
            static const char* _deliminators =
                "-+ .,!?()/\"\'{}\\";
        
            BString copy = word;
            char* str = copy.data();
                    
            char* token = 
            strtok(str, _whiteSpace);
            std::vector<BString> outer;
            
            // Loop through all remaining tokens
            while (token != nullptr) 
            {
                BString word =
                    BString(token).toLower();
                if (word.size()) {
                    words[word][path.id()];
                    outer.push_back(word);
                }
                token = strtok(nullptr, _whiteSpace); 
            }
            
            for (auto outerWord : outer)
            {
                BString copy = outerWord;
                char* str = copy.data();
                    
                char* token = 
                strtok(str, _deliminators);
                
                while (token != nullptr) 
                {
                    BString word =
                        BString(token).toLower();
                    if (word.size()) {
                        words[word][path.id()];
                    }
        
                    token = strtok(nullptr, _deliminators); 
                }
                
            }
        }
        
        virtual void setVariable(JSONPath start, const Type type, const BString& value)
        {
    
            start[type];
            
            if (&_log != &cnull)
                _log << start.toString() << endl;
            
            Path words = JSONPath::words();
            
            JSONPath path = start;
            
            // add all path keys except the
            // first (host)
            BString key;
                                
        
            while (!path.isRoot() &&
                    !path.parent().isRoot())
            {
                path = path.parent(key);
                
                if (!key.isDigitsOnly())
                {
                    if (key.startsWith("\"") &&
                        key.endsWith("\""))
                    {
                        key = key.substr(1, key.length() - 2).unescape();
                    }
                    addWord(words, key, start);
                }
    
            }
                

            
            switch (type)
            {
                case Type::UNDEFINED:
                {
                    JSONPath parent = start.parent(key);
                    parent.deleteKey(key);
                    break;
                }
                case Type::NULL_:
                {
                    start[Type::NULL_];
                    break;
                }
                case Type::BOOLEAN:
                case Type::INTEGER:
                case Type::NUMBER:
                {
                    start[type].setData(value);
                    
                    break;
                }
                case Type::STRING:
                {
                    start[type].setData(value);
                    
                    JSONPath path = start;

                    while (!path.isRoot() &&
                           !path.parent().isRoot())
                    {
                        addWord(words, value, path);
                        
                        path = path.parent();
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
        
        virtual void onobjectvalue(BeeFishJSON::Object* object, BeeFishJSON::ObjectKey* key, BeeFishJSON::JSON* json)
        override
        {

            JSONPath path = topPath();
            
            if (json->type() == Type::UNDEFINED)
            {
                path.deleteKey(key->value());
            }
            else
            {
                path = path[key->value()];
                setVariable(path, json->type(), json->value());

            }
            
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
                if (json->type() == Type::UNDEFINED)
                {
                    JSONPath::clear();
                    (*this)[Type::UNDEFINED];
                }
                else
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
