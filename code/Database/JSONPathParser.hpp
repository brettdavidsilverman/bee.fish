#ifndef BEE_FISH_DATABASE_JSON_PATH_PARSER_HPP
#define BEE_FISH_DATABASE_JSON_PATH_PARSER_HPP
#include "../json/json.h"
#include "../json/json-parser.h"

#include "../Authentication/authentication.h"

#include "JSONDatabase.hpp"
#include "JSONPath.hpp"
#include "NullStream.hpp"

namespace BeeFishDatabase {

using namespace BeeFishParser;
using namespace BeeFishScript;
using namespace BeeFishJSON;
using namespace BeeFishAuthentication;

class JSONPathParser :
    public JSONParser
{
private:
    Authentication& _auth;
    JSONPath _start;
    ostream& _log;
    vector<JSONPath> _pathStack;
    vector<Index> _indexStack;
    vector<BString> _keyStack;
    vector<Type> _typeStack;
    JSONPath _string;
    BString _partWord;
    Index _stringPageIndex;
    bool _indexString;


public:
    using JSONParser::read;
    
    JSONPathParser(Authentication& auth, JSONPath path, Match& match, ostream& log = cnull) :
        JSONParser(match),
        _auth(auth),
        _start(path),
        _log(log)
    {
        JSONDatabase& database = path.database();
        database._onword =
        [this](JSONPath& path, const BString& word)
        {
            JSONPathParser::log(path, word);
        };
    }

    JSONPathParser(Authentication& auth, JSONPath path, ostream& log = cnull) :
        JSONParser(),
        _auth(auth),
        _start(path),
        _log(log)
    {
        JSONDatabase& database = path.database();
        database._onword =
        [this](JSONPath& path, const BString& word)
        {
            JSONPathParser::log(path, word);
        };
    }

    virtual ~JSONPathParser()
    {


    }

    JSONPath start() {
        return _start;
    }

private:



    virtual void setVariable(JSONPath start, const Type type, const BString& value)
    {
        BString hash = value;


        switch (type)
        {
        case Type::UNDEFINED:

            start.setUndefined();
            // Dont log undefined
            return;
        case Type::NULL_:
            start.setNull();
            hash = "null";
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
            // String logs itself, simply return
            return;
        case Type::ARRAY:
            start.setArray();
            break;
        case Type::OBJECT:
            start.setObject();
            break;
        default:
            throw std::logic_error("JSONPathParser::setVariable");
        }

        log(start, hash);


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
                path = path[topIndex() + 1];
            }

        }

        if (type == Type::OBJECT)
            path.setObject();
        else if (type == Type::ARRAY)
            path.setArray();

        _pathStack.push_back(path);
        if (type == Type::ARRAY)
            _indexStack.push_back(0);
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
        if (key == "my")
            _keyStack.push_back(_auth.userId());
        else
            _keyStack.push_back(key);
    }

    void pop_back_key()
    {
        _keyStack.pop_back();
    }

    JSONDatabase& database()
    {
        return _start.database();
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

        if (json->type() != Type::STRING)
        {
            JSONPath path = topPath();

            path = path[key->value()];
            setVariable(path, json->type(), json->value());

            pop_back_key();
        }

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
        if (json->type() != Type::STRING)
        {
            JSONPath path = topPath();

            Index& index = topIndex();

            path = path[++index];

            setVariable(path, json->type(), json->value());
        }
    }

    virtual void onendarray(BeeFishJSON::JSON* match)
    override
    {

        Index length = topIndex();

        topPath().truncateArray(length);

        pop_back_path();

    }

    virtual void onbeginstring(BeeFishJSON::String* match)
    override
    {

        if (_pathStack.size() == 0)
            _string = _start;
        else {
            JSONPath path = topPath();
            if (topType() == Type::OBJECT)
            {
                _string = path[topKey()];
                pop_back_key();
            }
            else if (topType() == Type::ARRAY)
            {
                Index& index = topIndex();
                _string = path[++index];
            }
        }
        _stringPageIndex = 0;

    }

    virtual void onpartstring(const BString& partString)
    override
    {
        if (_stringPageIndex == 0)
            _indexString = !partString.isData();

        _string.setString(
            partString,
            _stringPageIndex++,
            _indexString,
            _partWord
        );
    }

    virtual void onendstring(BeeFishJSON::String* match)
    override
    {
        _string.endString(
            _stringPageIndex,
            _indexString,
            _partWord
        );

    }

    virtual void onvalue(BeeFishJSON::JSON* json)
    override
    {

        if (_pathStack.size() == 0)
        {
            if (json->type() != Type::STRING)
            {
                JSONPath start = _start;
                setVariable(
                    start,
                    json->type(),
                    json->value()
                );
            }
        }



    }

    void log(JSONPath& path, const BString& value)
    {
        if (&_log != &cnull) {
            
            _log << BeeFishDate::getDateTime()
                 << " "
                 << path.toString(_auth);
                 
            if (value.size())
            {
                _log << "#"
                     << value;
            }
            
            _log << endl;
        }
    }


};



}

#endif
