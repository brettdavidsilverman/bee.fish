#ifndef BEE_FISH_DATABASE_JSON_PATH_HPP
#define BEE_FISH_DATABASE_JSON_PATH_HPP
#include "../Id/Id.hpp"
#include "../json/json.h"
#include "../json/json-parser.h"
#include "../Database/DatabaseBase.hpp"
#include "../Database/Path.hpp"
#include "../web-request/web-request.h"
#include "../Authentication/authentication.h"

#include "NullStream.hpp"
#include "JSONIndex.hpp"

namespace BeeFishDatabase {

using namespace BeeFishPowerEncoding;
using namespace BeeFishParser;
using namespace BeeFishScript;
using namespace BeeFishJSON;
using namespace BeeFishAuthentication;


class JSONPath :
    public Path
{
protected:
    LockFile::ScopedLock* _lock = nullptr;

public:
    inline static const Index VALUE = 0;
    inline static const Index PROPERTIES = 1;
    inline static const Index POSITIONS = 2;
    inline static const Index CASCADE = 3;
    inline static const Index ID = 4;
    inline static const Index INDEXED = 5;
    inline static const Index CHILDREN = 6;
public:

#ifdef JSON_INDEX
    typedef Index Id;
#else
    typedef BeeFishId::Id Id;
#endif


    using Path::contains;
    using Path::clear;



    JSONPath()
    {
    }

    JSONPath(JSONDatabase& database, Index index) :
        Path(database, index)
    {

    }

#ifndef JSON_INDEX
    JSONPath(JSONDatabase& database, Id id) :
        Path(
            database,
            database.objects()
            [id].getData<Index>()
        )
    {

    }
#endif

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

    virtual PathBase* copy() const
    override
    {
        return new JSONPath(*this);
    }

    JSONDatabase& database()
    {
        return (JSONDatabase&)
               Path::database();
    }


    Id id() const
    {
        //return index();

        Path path = *this;
        path = path[ID];

#ifdef JSON_INDEX
        return path.getData<Index>();
#else
        Id id;
        path.getData(id);

        return id;
#endif

    }

#ifdef JSON_INDEX
    Id setId()
    {
        Id id = index();
        return setId(id);
    }

    Id setId(const Id& id)
    {
        Path path = *this;
        path = path[ID];

        path.setData<Id>(id);

        return id;
    }

#else
    Id setId(const Id& id = Id(""))
    {
        Path path = *this;
        path = path[ID];

        path.setData<Id>(id);

        return id;
    }
#endif

    bool hasId()
    {
        Path path = *this;
        path = path[ID];
        return path.hasData();
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

        setType(Type::OBJECT);

        // This creates the child and
        // indexes its properties
        Index position =
            getObjectPropertyPosition(property);


        JSONPath json = getChildren()[position];

        return json;

    }

    JSONPath operator [] (const Index& index)
    {

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

                        if (!child.hasId())
                            child.setId();

                        child.indexProperties();
                    }
                }
                else
                    throw runtime_error("Index out of bounds");

                children.setData<Index>(count);
            }
        }

        JSONPath json = getChildren()[index];


        return json;
    }

    void indexProperties()
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
                while (!json.isUserRoot())
                {
                    BString property;
                    Type keyType;
                    json = json.parent(property, keyType);

                    if (keyType == Type::STRING) {
                        addWords(property);
                    }

                }
            }
        }


    }

protected:

    void addObject()
    {

        Id id = JSONPath::id();

        Path objects =
            database().objects();

        // Add this child to all parents
        JSONPath json = *this;
        while (!json.isRoot())
        {
            objects[json.id()][id];
            json = json.parent();
        }

        objects[id].setData<Index>(index());
    }

    void removeObject()
    {

        Id id = JSONPath::id();

        Path objects =
            database().objects();

        JSONPath json = *this;

        // Remove this child from its parents
        while (!json.isRoot() )
        {
            objects[json.id()].clear(id);
            json = json.parent();
        }

        // Remove this child
        objects.clear(id);
    }

    void setType(Type type)
    {
        LockFile::ScopedLock lock(database());

        if (!hasData())
        {
            setData<Type>(type);

            if (!hasId())
                setId();

            addObject();

        }
        else if (JSONPath::type() != type)
        {
            Id id = JSONPath::id();

            clear();
            setData<Type>(type);

            setId(id);

            addObject();
        }


    }


public:


    void setUndefined()
    {
        LockFile::ScopedLock lock(database());

        if (type() == Type::UNDEFINED) {
            return;
        }

        if (!isRoot())
        {
            BString property;
            JSONPath parent =
                JSONPath::parent(property);
            if (parent.type() == Type::OBJECT)
            {
                parent.deleteProperty(property);
            }
            else if (parent.type() == Type::ARRAY)
                setType(Type::NULL_);
            else
                setType(Type::UNDEFINED);
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

    void setInteger(const Index& value)
    {
        setInteger(std::to_string(value));
    }

    void setNumber(const BString& value)
    {
        LockFile::ScopedLock lock(database());
        Path path = *this;
        setType(Type::NUMBER);
        path[VALUE].setData<BString>(value);
    }

    Index getInteger()
    {
        Path path = *this;
        stringstream stream;
        stream << path[VALUE].getStringData();
        Index number;
        stream >> number;
        return number;
    }

    void setString(const BString& value)
    {

        BString partWord;
        setString(
            value,
            0,
            true,
            partWord
        );

        endString(1, true, partWord);

    }

    void setString(
        const BString& value,
        Index pageIndex,
        bool indexData,
        BString& partWord
    )
    {

        if (!_lock)
            _lock = new LockFile::ScopedLock(database());

        Path path = *this;
        setType(Type::STRING);

        bool partChanged = false;
        Index max = 0;
        if (!path[VALUE].isDeadEnd())
            max = path[VALUE].max<Index>();

        BString current;
        if (pageIndex <= max &&
                !path[VALUE].isDeadEnd())
        {
            if ((current = path[VALUE][pageIndex].getStringData())
                    != value)
            {
                if (path[VALUE].hasData() &&
                        path[VALUE].getData<bool>())
                {

                    BString partRemoveWord = partWord;

                    for (Index i = pageIndex;
                            i <= max;
                            ++i)
                    {
                        BString current =
                            path[VALUE][i].getStringData();
                        removeWords(current, partRemoveWord, false, i == max);
                        path[VALUE].clear(i);
                    }

                }
                partChanged = true;
            }

        }
        else
            partChanged = true;


        path[VALUE][pageIndex].setData<BString>(value);

        if (indexData && partChanged) {
            addWords(value, partWord, false, false);
        }


    }

    void endString(
        Index pageCount,
        bool indexData,
        BString& partWord
    )
    {
        Path path = *this;

        addWords("", partWord, false, true);

        path = path[VALUE];
        Index max = 0;

        if (!path.isDeadEnd())
            max = path.max<Index>();

        bool currentIndexData =
            path.hasData() &&
            path.getData<bool>();


        for (Index i = pageCount;
                i <= max;
                ++i)
        {
            if (currentIndexData)
            {
                BString current =
                    path[i].getStringData();
                removeWords(current, partWord, false, i == max);
            }
            path.clear(i);
        }

        path.setData<bool>(indexData);

        partWord = "";

        delete _lock;
        _lock = nullptr;

    }

public:

    BString getString() const
    {
        Path path = *this;
        if (path[VALUE].isDeadEnd())
            return "";
        return path[VALUE][0].getStringData();
    }

    BString getValue() const
    {
        Path path = *this;
        if (path[VALUE].hasData())
            return path[VALUE].getStringData();
        return "";
    }

    void setObject()
    {
        LockFile::ScopedLock lock(database());

        setType(Type::OBJECT);

    };

    void setArray()
    {
        LockFile::ScopedLock lock(database());
        setType(Type::ARRAY);

    }

    void truncateArray(Index size)
    {

        Path children = getChildren();
        if (!children.isDeadEnd())
        {
            Index max = children.max<Index>();
            for (Index i = size + 1; i <= max; ++i)
            {
                JSONPath child = children[i];
                child.clear();
                children.clear(i);
            }
        }
        children.setData<Index>(size);


    }

    JSONPath parent() const {
        BString key;
        Type type;
        return parent(key, type, false);
    }

    JSONPath parent(BString& key, bool fetchKey = true) const
    {
        Type type;
        return parent(key, type, fetchKey);
    }

    JSONPath parent(BString& key, Type& keyType, bool fetchKey = true) const {
        Path path = *this;

        Index position = -1;


        path = path.parent(position);


        Index seperator;

        path = path.parent(seperator);

        Type type =
            path.getData<Type>();

        if (fetchKey)
        {
            if (type == Type::ARRAY)
            {
                key = std::to_string(position);
                keyType = Type::INTEGER;
            }
            else if (type == Type::OBJECT)
            {
                JSONPath object = path;
                key = object.getObjectProperty(position);
                keyType = Type::STRING;
            }

        }

        return path;
    }

    bool isUserRoot()
    {
        if (!isRoot()) {
            BString userId;

            JSONPath parent =
                JSONPath::parent(userId);

            if (
                database()
                .users()
                .contains(userId)
            )
            {
                return true;
            }
            else
                return parent.isRoot();
        }

        return true;

    }



    BString toString(Authentication& auth) {
        JSONPath path = *this;
        BString string;
        BString key;
        const BString& userId = auth.userId();

        while (!path.isRoot())
        {
            Type keyType = Type::STRING;
            path = path.parent(key, keyType);

            if (path.isUserRoot())
            {
                if (key == userId)
                {
                    key = "my";
                }
                else if ( database()
                          .users()
                          .contains(key) )
                {
                    return "";
                }
            }


            if (key != "my" && !path.isRoot()) {
                if (keyType == Type::STRING &&
                        key.isDigitsOnly())
                {
                    key = BString("\"") + key + BString("\"");
                }
                else
                    key = key.encodeURI();

            }

            string =
                key +
                BString("/") +
                string;

        }

        if (string.size())
            string.pop_back();

        const BString http =
            BString("{HTTP}").encodeURI();

        if (string.contains(http))
        {
            string = string.substr(
                         0,
                         string.find(http) - 1
                     );
        }


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

    static JSONPath fromString(
        Authentication& auth,
        JSONDatabase& database,
        const BeeFishWeb::URL& url,
        const BString& method = "GET"
    )
    {
        return fromString(
                   auth,
                   database,
                   url.origin(),
                   url,
                   method
               );
    }

    static JSONPath fromString(
        Authentication& auth,
        JSONDatabase& database,
        const BString& origin,
        const BeeFishWeb::URL& url,
        const BString& method = "GET"
    )
    {

        JSONPath originPath =
            database.origin(origin);

        std::vector<BString> paths =
            url.paths();

        // Loop through all tokens
        JSONPath path = originPath;
        bool success = true;
        bool first = true;

        const BString& userId = auth.userId();

        for (BString key : paths)
        {
            key = key.decodeURI();

            if (first && (key == "my" || key == userId))
            {
                if (path.contains(userId) || method == "POST") {
                    path = path[userId];
                    continue;
                }

            }

            if ( database.users().contains(key) && key != userId)
            {
                success = false;
                break;
            }


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
                        key.endsWith("\"") &&
                        key.length() > 1
                   )
                {
                    key =
                        key.substr(
                            1,
                            key.length() - 2
                        );
                }

                if (path.contains(key) || method == "POST")
                    path = path[key];
                else {
                    success = false;
                    break;
                }

            }

            first = false;



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
            .contains(path);

        return contains;

    }

    bool contains(const char* property)
    {
        return contains(BString(property));
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
                propertyPath[index()];

                // Update positions
                position = ++getChildren();

                getPositions()[position].setData<Index>(propertyPath.index());
                objectPropertyPath.setData<Index>(position);

                JSONPath childPath = getChildren()[position];
                childPath.setId();
                childPath.indexProperties();

            }
        }



        position = objectPropertyPath.getData<Index>();


        return position;
    }


    BString getObjectProperty(Index position)
    {

        Path path = getPositions();

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

        return path.getData<Type>();
    }

public:
    void clear()
    {
        switch (type())
        {
        case Type::STRING:
        {
            removeWords();
            Path path = *this;
            path.clear(VALUE);
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
            Path clearChildren = getChildren();
            for (auto index : iterable)
            {
                JSONPath item = (*this)[index];
                item.clear();
                clearChildren.clear(index);
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


        removeObject();

        Path::clear();

    }

public:
    void deleteProperty(const BString& property)
    {

        if (!contains(property))
            return;

        JSONPath json = (*this)[property];

        // Remove parent properties
        JSONPath path = json;

        while (!path.isUserRoot())
        {
            BString property;
            Type keyType;
            path = path.parent(property, keyType);

            if (keyType == Type::STRING)
                json.removeWords(property, true);

        }


        Index position = getObjectPropertyPosition(property);
        Path propertyPath = properties()[property];

        json.clear();

        if (propertyPath.contains(index()))
        {
            propertyPath.clear(index());
            if (propertyPath.isDeadEnd())
                properties().clear(property);
        }


        getObjectProperties().clear(propertyPath);
        getChildren().clear(position);
        //--getChildren();
    }

private:
    void addWords(const BString& word, bool wholeWord = true)
    {
        BString partWord = "";
        addWords(word, partWord, wholeWord, true);
    }

    void addWords(const BString& value, BString& partWord, bool isWholeWord = false, bool isFinalWord = false)
    {
        Path words = database().words();

        std::vector<BString> tokens =
            value.tokenise(
                partWord,
                isFinalWord
            );

        if (isWholeWord &&
                find(
                    tokens.begin(),
                    tokens.end(),
                    value.toLower()
                ) == tokens.end())
        {
            tokens.push_back(value.toLower());
        }

        for (auto word : tokens)
        {

            Path wordPath = words[word];

            JSONPath json = *this;

            while (!json.isUserRoot())
            {
                ++wordPath[json.id()];

                json = json.parent();
            }

            if (database()._onword)
            {
                database()._onword(*this, word);
            }

        }
    }

    void removeWords() {

        Path path = *this;
        Path strings = path[VALUE];
        bool remove =
            strings.hasData() &&
            strings.getData<bool>();

        if (remove) {
            Iterable<Index> parts(strings);

            BString partRemoveWords;

            for (auto it = parts.begin(); it != parts.end(); )
            {
                BString current =
                    strings[*it].getStringData();

                ++it;

                removeWords(current, partRemoveWords, false, it == parts.end());

            }

        }
    }

    void removeWords(const BString& value, bool wholeWord)
    {

        BString partRemoveWords;
        removeWords(value, partRemoveWords, wholeWord, true);

    }

    void removeWords(const BString& value, BString& partRemoveWords, bool wholeWord, bool isFinalPart)
    {
        Path words = database().words();
        Path objects = database().objects();

        std::vector<BString> tokens =
            value.tokenise(
                partRemoveWords,
                isFinalPart
            );

        if (wholeWord &&
                find(
                    tokens.begin(),
                    tokens.end(),
                    value.toLower()
                ) == tokens.end())
        {
            tokens.push_back(value.toLower());
        }

        for (auto word : tokens)
        {
            if (words.contains(word))
            {
                Path wordPath = words[word];

                JSONPath json = (*this);

                while  (!json.isUserRoot())
                {

                    if (wordPath.contains(json.id()))
                    {

                        Index count = --wordPath[json.id()];

                        if (count == 0)
                        {

                            wordPath.clear(json.id());
                            if (wordPath.isDeadEnd())
                                words.clear(word);
                        }
                    }

                    json = json.parent();
                }



            }


        }

    }

public:
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
            Path strings = path[VALUE];
            Iterable<Index> iterable(strings);

            out << "\"";
            BString partWord;
            for (const auto index : iterable)
            {
                BString string =
                    strings[index].getStringData();

                string.escape(out, partWord);
            }


            out << "\"";



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
            ++(*this);
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
JSONPath JSONDatabase::origin(const BString& origin) const
{
    return json()[origin];
}

PowerEncoding& operator << (PowerEncoding& output, const JSONPath& json)
{
    return output << json.id();
}




}



#endif
