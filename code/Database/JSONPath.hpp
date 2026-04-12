#ifndef BEE_FISH_DATABASE_JSON_PATH_HPP
#define BEE_FISH_DATABASE_JSON_PATH_HPP
#include "../json/json.h"
#include "../json/json-parser.h"
#include "../Database/DatabaseBase.hpp"
#include "../Database/Path.hpp"
#include "../web-request/web-request.h"
#include "NullStream.hpp"
#include "JSONIndex.hpp"

namespace BeeFishDatabase {

using namespace BeeFishPowerEncoding;
using namespace BeeFishParser;
using namespace BeeFishScript;
using namespace BeeFishJSON;



class JSONPath :
    public Path
{
public:


public:
    inline static const Index VALUE = 0;
    inline static const Index PROPERTIES = 1;
    inline static const Index POSITIONS = 2;
    inline static const Index CASCADE = 3;
    inline static const Index CHILDREN = 4;
public:
    typedef Index Id;

    using Path::contains;
    using Path::clear;


    JSONPath()
    {
    }

    JSONPath(JSONDatabase& database, Index index) :
        Path(database, index)
    {

    }

    JSONPath(JSONDatabase& database, const Path& start) :
        Path(database, start.index())
    {

    }

    JSONPath(Path start) :
        JSONPath((JSONDatabase&)start.database(), start)
    {
    }

    JSONPath(const JSONPath& source) :
        Path(source)
    {

    }

    JSONDatabase& database()
    {
        return (JSONDatabase&)
               Path::database();
    }

    Id id()
    {
        return index();
    }

    Path properties()
    {
        return database().properties();
    }

    Path words()
    {
        return database().words();
    }


    JSONPath operator [] (const BString& property)
    {
       // LockFile::ScopedLock lock(database());

        setType(Type::OBJECT);

        Index position =
            getObjectPropertyPosition(property);

        assert(position > 0);

        JSONPath json = getChildren()[position];

        json.cascadeProperties();

        return json;

    }

    JSONPath operator [] (const Index& index)
    {
       // LockFile::ScopedLock lock(database());
        if (index < 1)
        {
            throw runtime_error("Index out of bounds");
        }

        Path children = getChildren();
        Index count = 0;

        if (type() == Type::UNDEFINED)
        {
            setType(Type::ARRAY);
        }

        if (children.hasData())
            count = children.getData<Index>();
            
        if (index > count)
        {
            LockFile::ScopedLock lock(database());
            
            if (children.hasData())
                count = children.getData<Index>();
                
            if (index > count)
            {
                if (type() == Type::ARRAY)
                {
                    while (index > count)
                    {
                        JSONPath child = children[++count];
                    }
                }
                else
                    throw runtime_error("Index out of bounds");

                children.setData<Index>(count);
            }
        }

        JSONPath json = getChildren()[index];

        json.cascadeProperties();

        return json;
    }

    void cascadeProperties()
    {

        Path path = *this;
        path = path[CASCADE];


        if (!path.hasData())
        {
            LockFile::ScopedLock lock(database());
            if (!path.hasData())
            {
                path.setData(true);

                JSONPath json = *this;
                while (!json.isRoot() &&
                       !json.parent().isRoot())
                {
                    BString property;
                    json = json.parent(property);
                    if (!property.isDigitsOnly())
                    {
                        if (property.startsWith("\"") &&
                                property.endsWith("\""))
                        {
                            property =
                                property.substr(1, property.length() - 2)
                                .unescape();
                        }
                        addWords(property, false);
                    }
                }
            }

        }
    }

protected:
    
    void addObject()
    {
        Path objects =
            database().objects();
            
        objects[*this];
        
        // Add this child
        objects[*this][OBJECT_CHILDREN][*this];

        // Add this child to all parents
        JSONPath json = *this;
        while (!json.isRoot())
        {

            objects[json]
               [OBJECT_CHILDREN]
               [*this];
            json = json.parent();
        }

    }
    
    void removeObject()
    {

        Path objects =
            database().objects();
            
        JSONPath json = *this;
        
    
        // Remove this child from its parents
        while (!json.isRoot())
        {
            objects[json][OBJECT_CHILDREN]
                .clear(*this);
            json = json.parent();
        }
        
        // Remove this child
        objects.clear(*this);
    }
    
    void setType(Type type)
    {
        if (!hasData())
        {
            
            setData<Type>(type);
            
            if (!isRoot())
                addObject();
            
        }
        else if (JSONPath::type() != type)
        {
            LockFile::ScopedLock lock(database());
            if (JSONPath::type() != type)
            {
                clear();
                assert(isDeadEnd());
                setData<Type>(type);
                if (!isRoot())
                    addObject();
                
            }
        }

        assert(JSONPath::type() == type);

    }

public:


    void setUndefined()
    {
        LockFile::ScopedLock lock(database());

        if (type() == Type::UNDEFINED)
            return;

        setType(Type::UNDEFINED);

        if (!isRoot())
        {
            BString property;
            JSONPath path = parent(property);
            if (path.type() == Type::OBJECT)
                path.deleteProperty(property);
            else if (path.type() == Type::ARRAY)
                setType(Type::NULL_);
        }
    }

    void setNull()
    {
        LockFile::ScopedLock lock(database());
        setType(Type::NULL_);
    }


    void setBoolean(const BString& value)
    {
        LockFile::ScopedLock lock(database());
        Path path = *this;
        setType(Type::BOOLEAN);
        path[VALUE].setData<BString>(value);
    }

    void setInteger(const BString& value)
    {
        LockFile::ScopedLock lock(database());
        Path path = *this;
        setType(Type::INTEGER);
        path[VALUE].setData<BString>(value);
    }

    void setNumber(const BString& value)
    {
        LockFile::ScopedLock lock(database());
        Path path = *this;
        setType(Type::NUMBER);
        path[VALUE].setData<BString>(value);
    }

    void setString(const BString& value)
    {
        LockFile::ScopedLock lock(database());
        Path path = *this;
        setType(Type::STRING);
        if (path[VALUE].hasData())
        {
            BString current = path[VALUE].getStringData();
            if (current != value)
                removeWords(current, true);
        }
        if (path[VALUE].setData<BString>(value))
            addWords(value, true);
    }

    void setObject()
    {
        LockFile::ScopedLock lock(database());
        setType(Type::OBJECT);
    }

    void setArray()
    {
        LockFile::ScopedLock lock(database());
        setType(Type::ARRAY);

    }

    JSONPath parent() {
        BString key;
        return parent(key);
    }

    JSONPath parent(BString& key) {
        Path path = *this;

        Index position = -1;

        assert(!path.isRoot());

        path = path.parent(position);

        assert(!path.isRoot());

        Index seperator;

        path = path.parent(seperator);
        assert(seperator == CHILDREN);

        Type type =
            path.getData<Type>();

        if (type == Type::ARRAY)
        {
            stringstream stream;
            stream << position;
            key = stream.str();
        }
        else if (type == Type::OBJECT)
        {
            JSONPath object = path;
            key = object.getObjectProperty(position);
            if (key.isDigitsOnly())
            {
                key = BString("\"") + key + BString("\"");
            }
            else
                key = key.escape();
        }
        else
            assert(false);

        return path;
    }



    BString toString() {
        JSONPath path = *this;
        BString string;
        BString key;
        while (!path.isRoot())
        {
            path = path.parent(key);
            BString newString =
                key + BString("/") + string;
            string = newString;
        }
        if (string.size())
            string.pop_back();
        return string;
    }

    class PathNotFoundException : public std::runtime_error
    {
        const BString _url;

    public:
        // Pass the message to the base class constructor
        PathNotFoundException(const BString& url)
            : std::runtime_error(url),
              _url(url)
        {
        }

        const BString& url() {
            return _url;
        }
    };

    static JSONPath fromString(JSONDatabase& database, const BeeFishWeb::URL& url, const BString& method = "GET")
    {
        return fromString(
            database,
            url.origin(),
            url
        );
    }
    
    static JSONPath fromString(JSONDatabase& database, const BString& host, const BeeFishWeb::URL& url, const BString& method = "GET")
    {
        JSONPath hostPath =
            database.host(host);

        std::vector<BString> paths =
            url.path().split('/');
        
        // Loop through all tokens
        JSONPath path = hostPath;
        bool success = true;
        
        for (const BString& token : paths)
        {

            BString key = token;

            if (key.size()) {
                key = key.decodeURI();
                if (key.isDigitsOnly()) {
                    Index index = atol(key.c_str());
                    if (path.contains(index))
                        path = path[index];
                    else {
                        success = false;
                        break;
                    }
                }
                else {
                    if (key.startsWith("\"") &&
                            key.endsWith("\"")
                       )
                    {
                        key =
                            key.substr(
                                1,
                                key.length() - 2
                            );
                    }
                    key = key.unescape();
                    if (path.contains(key) || method == "POST")
                        path = path[key];
                    else {
                        success = false;
                        break;
                    }
                }



            }
        }

        if (!success) {
            throw PathNotFoundException(url);
        }

        return path;
    }

    bool contains(const BString& property)
    {
        if (type() != Type::OBJECT)
            return false;

        Path path =
            properties();

        if (!path.contains(property))
        {
            return false;
        }

        path = path[property];


        bool contains =
            getObjectProperties()
            .contains(path.index());

        return contains;

    }

    bool contains(const Index& index)
    {
        if ((type() != Type::ARRAY) &&
                (type() != Type::OBJECT))
            return false;

        Path children = getChildren();

        if (!children.contains(index))
        {
            return false;
        }


        return true;

    }

    Index getObjectPropertyPosition(const BString& property)
    {


        // Get the property index
        Path propertyPath =
            properties()[property];

        // Get the position
        Index position;

        Path objectPropertyPath =
            getObjectProperties()[propertyPath];

        if (!objectPropertyPath.hasData())
        {
            LockFile::ScopedLock lock(database());
            if (!objectPropertyPath.hasData())
            {
                // New property
                // Update the properties counter
                propertyPath[*this];

                // Update positions
                position = ++getChildren();

                getChildren()[position];
                getPositions()[position].setData<Index>(propertyPath.index());
                objectPropertyPath.setData<Index>(position);

                // add all path properties except the
                // first (host)


                JSONPath path = *this;
                while (!path.isRoot() &&
                       !path.parent().isRoot())
                {
                    BString property;
                    path = path.parent(property);
                    if (!property.isDigitsOnly())
                    {
                        if (property.startsWith("\"") &&
                                property.endsWith("\""))
                        {
                            property =
                                property.substr(1, property.length() - 2)
                                .unescape();
                        }
                        addWords(property, false);
                    }

                }
            }

        }

        position = objectPropertyPath.getData<Index>();


        return position;
    }


    BString getObjectProperty(Index position)
    {

        Path path = getPositions();

        assert(path.contains(position));

        path = path[position];

        Index index = path.getData<Index>();

        BString property;
        Path propertyPath(database(), index);
        propertyPath.parent(property);

        return property;

    }

    JSONPath operator [] (const char* key)
    {
        return (*this)[BString(key)];
    }

    friend ostream& operator << (ostream& out, const JSONPath& json)
    {
        JSONPath path(json);

        path.write(out, 0);

        return out;
    }

    BString tabs(Index tabCount) const
    {
        return string(tabCount * TAB_SPACES, ' ');
    }


    Path getChildren()
    {

        Path path = *this;

        path << CHILDREN;

        return path;
    }

    Path getPositions()
    {

        Path path = *this;

        path << POSITIONS;

        return path;
    }

    Path getObjectProperties()
    {
//setType(Type::OBJECT);
        Path path = *this;
        path << PROPERTIES;
        return path;
    }

    Index getPropertyCount()
    {
        Path path = *this;

        if (type() != Type::OBJECT)
            return 0;

        Path children = getChildren();

        if (children.hasData())
        {
            Index count =
                children.getData<Index>();
            return count;
        }

        return 0;
    }

    Type type() const {
        Path path(*this);

        if (!path.hasData())
            return Type::UNDEFINED;

        Type type =
            path.getData<Type>();
        return type;
    }

public:

    virtual void clear() override
    {


        switch (type())
        {
        case Type::STRING:
        {
            Path path = *this;
            BString value =
                path[VALUE].getStringData();
            removeWords(value, true);
        }
        case Type::UNDEFINED:
        case Type::NULL_:
        case Type::BOOLEAN:
        case Type::INTEGER:
        case Type::NUMBER:
            break;
        case Type::ARRAY:
        {

            Path children = getChildren();
            Iterable<Index> iterable(children);

            for (auto index : iterable)
            {
                JSONPath item = (*this)[index];
                item.clear();
                getChildren().clear(index);
            }

            break;
        }
        case Type::OBJECT:
        {
            Path children = getChildren();
            Iterable<Index> iterable(children);

            for (auto index : iterable)
            {
                BString property = getObjectProperty(index);
                deleteProperty(property);
            }

            break;

        }
        case Type::UNKNOWN:
            throw runtime_error("Unknown type");
        }

        assert(getChildren().isDeadEnd());

        if (!isRoot()) {
            removeObject();
        }
            
        
        Path::clear();
        assert(isDeadEnd());

    }

    void deleteProperty(const BString& property)
    {

        //if (!contains(property))
        //    return;

        JSONPath json = (*this)[property];


        json.removeWords(property, false);

        // Remove parent properties
        JSONPath path = json;
        while (!path.isRoot() &&
               !path.parent().isRoot())
        {
            BString property;
            path = path.parent(property);
            if (!property.isDigitsOnly())
            {
                if (property.startsWith("\"") &&
                        property.endsWith("\""))
                {
                    property = property.substr(1, property.size() - 2);
                    property = property.unescape();
                }
                removeWords(property, false);
            }

        }


        Index position = getObjectPropertyPosition(property);
        Path propertyPath = properties()[property];

        json.clear();

        if (propertyPath.contains(*this))
        {
            propertyPath.clear(*this);
            if (propertyPath.isDeadEnd())
                properties().clear(property);
        }


        getObjectProperties().clear(propertyPath);
        getChildren().clear(position);
        --getChildren();
    }

    void addWords(const BString& word, bool addToParents)
    {

        Path words = database().words();
        Path objects = database().objects();

        Path path = *this;

        std::vector<BString> tokens =
            word.tokenise();


        for (auto word : tokens)
        {
            Path wordPath = words[word];
            JSONPath json = *this;

            if (addToParents)
            {
                while (!json.isRoot() &&
                       !json.parent().isRoot())
                {
                    objects[json][OBJECT_WORDS][wordPath];
                    wordPath[json];
                    json = json.parent();
                }
            }
            else {
                objects[json][OBJECT_WORDS][wordPath];
                wordPath[json];
            }

        }
    }

    void removeWords(const BString& value, bool removeFromParents)
    {
        Path words = database().words();
        Path objects = database().objects();

        std::vector<BString> tokens =
            value.tokenise();

        for (auto word : tokens)
        {
            if (words.contains(word))
            {

                Path wordPath = words[word];

                JSONPath json = (*this);


                if (removeFromParents)
                {
                    while  (!json.isRoot() &&
                            !json.parent().isRoot())
                    {
                        Path object =
                            objects
                            [json]
                            [OBJECT_WORDS];
                            
                        if (object.contains(wordPath))
                        {
                            wordPath.clear(json);
                            object.clear(wordPath);
                        }

                        json = json.parent();
                    }

                }
                else
                {
                    Path object =
                        objects
                        [json]
                        [OBJECT_WORDS];
                    if (object.contains(wordPath))
                    {
                        wordPath.clear(json);
                        object.clear(wordPath);
                    }
                }


                if (wordPath.isDeadEnd())
                {
                    words.clear(word);
                }

            }


        }

    }
/*
    std::vector<BString> tokenise(BString word)
    {
        static const char* _deliminators =
            " \r\n\v\t()\".,!?{}+:; /";

        char* str = word.data();
        char *saveptr;

        char* token =
            strtok_r(str, _deliminators, &saveptr);

        std::vector<BString> words;

        // Loop through all remaining tokens
        while (token != nullptr)
        {
            if (strlen(token))
            {
                words.push_back(token);
            }
            token = strtok_r(nullptr, _deliminators, &saveptr);
        }


        // 1. Sort words
        std::sort(words.begin(), words.end());

        // 2. Remove consecutive duplicates: v becomes {1, 2, 3, 4, 5, ?, ?, ?, ?, ?}
        //    'last' points to the first '?'
        auto last = std::unique(words.begin(), words.end());

        // 3. Erase the extra elements
        words.erase(last, words.end());

        return words;
    }

*/

    virtual void write(ostream& out, Index tabCount = 0)
    {

        Type type = JSONPath::type();

        Path path = *this;

        switch (type) {
        case Type::UNDEFINED:
            out << "undefined";
            break;
        case Type::NULL_:
            out << "null";
            break;
        case Type::BOOLEAN:
        case Type::INTEGER:
        case Type::NUMBER:
        {
            BString value =
                path[VALUE].getStringData();
            out << value;
            break;
        }
        case Type::STRING:
        {
            BString string =
                path[VALUE].getStringData();

            out << "\""
                << string.escape()
                << "\"";

            break;
        }
        case Type::ARRAY:
        {
            out << "[";

            Index count = 0;

            path = getChildren();

            Iterable<Index> arrayIndexes(path);

            if (path.hasData())
                count = path.getData<Index>();

            if (count > 1) {
                out << "\n";
                out << tabs(tabCount + 1);
            }

            for (auto index : arrayIndexes)
            {

                JSONPath item = (*this)[index];

                Index _tabCount = tabCount + 1;

                if (count == 1)
                    _tabCount--;

                if (item.type() == Type::UNDEFINED)
                {
                    out << "null";
                }
                else
                {
                    item.write(
                        out,
                        _tabCount
                    );
                }

                if (index < count)
                {
                    out << ",\n" << tabs(tabCount + 1);
                }
            }

            if (count > 1) {
                out << "\n";
                out << tabs(tabCount);
            }

            out << "]";

            break;
        }
        case Type::OBJECT:
        {
            Path children = getChildren();

            if (children.isDeadEnd())
            {
                out << "{}";
                break;
            }

            out << "{";
            Index count = 0;

            out << "\n";

            BString key;
            for (Iterator iterator = begin(); iterator != end();)
            {
                key = *iterator;

                ++count;

                out << tabs(tabCount + 1)
                    << "\""
                    << key.escape()
                    << "\": ";

                JSONPath value =
                    (*this)[key];

                value.write(out, tabCount + 1);

                if (++iterator != end())
                    out << ",\n";

            }



            if (count > 0) {
                out << "\n";
                out << tabs(tabCount);
            }

            out  << "}";
            break;
        }
        default:
            throw std::logic_error("JSONPath::write");
        }

    }

public:

    class Iterator {
    protected:
        JSONPath* _jsonPath; // The underlying pointer that the iterator wraps
        BString _item;
        Stack _stack;
        bool _isEnd = true;
        Path _children;
        Index _position;
    public:


        // Iterator traits (required for STL compatibility in C++17 and earlier)
        using iterator_category = std::forward_iterator_tag;
        using value_type        = BString;
        using difference_type   = std::ptrdiff_t;
        using reference         = const BString&;

        Iterator(JSONPath* path, bool isEnd = false)
            : _jsonPath(path)
        {

            if (isEnd) {
                _isEnd = true;
            }
            else {
                _children = _jsonPath->getChildren();
                _isEnd = !_children.next<Index>(_stack, _position);
                if (!_isEnd) {
                    _item = _jsonPath->getObjectProperty(_position);
                }
            }
        }


        // Dereference operator (*)
        reference operator*() const
        {
            return _item;
        }

        // Prefix increment operator (++)
        Iterator& operator++() {

            _isEnd = _isEnd or not
                     _children.next<Index>(
                         _stack,
                         _position
                     );

            if (!_isEnd)
                _item = _jsonPath->getObjectProperty(_position);

            return *this;
        }

        // Postfix increment operator (++)
        Iterator operator++(int) {
            Iterator tmp = *this;
            tmp._children.save();
            ++(*this);
            tmp._children.restore();
            return tmp;
        }

        friend bool operator == (
            const Iterator& a,
            const Iterator& b
        )
        {
            return  (a._isEnd == b._isEnd &&
                     a._jsonPath == b._jsonPath &&
                     a._stack == b._stack);
        }

        friend bool operator != (
            const Iterator& a,
            const Iterator& b
        )
        {
            return (a._isEnd != b._isEnd ||
                    a._jsonPath != b._jsonPath ||
                    a._stack != b._stack);
        }

        Index position() const {
            return _position;
        }


    };

    // Container methods to get iterators
    Iterator begin() {
        return Iterator(this);
    }

    // Points one past the last element
    Iterator end() {
        Iterator iterator(this, true);
        return iterator;
    }

};


// Declared in JSONDatabase.hpp
JSONPath JSONDatabase::json() const
{
    return _json;
}

// Declared in JSONDatabase.hpp
JSONPath JSONDatabase::host(const BString& host) const
{
    return json()[host];
}

PowerEncoding& operator << (PowerEncoding& output, const JSONPath& json)
{
    return output << json.index();
}



}

#endif
