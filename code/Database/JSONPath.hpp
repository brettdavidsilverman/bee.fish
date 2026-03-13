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
        
        inline static const Index TYPE = 0;
        inline static const Index VALUE = 1;
        inline static const Index PROPERTIES = 2;
        inline static const Index POSITIONS = 3;
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

        
        JSONPath operator [] (const BString& key)
        {
            lock();
            
            setType(Type::OBJECT);

            Index position =
                getObjectPropertyPosition(key);
            assert(position > 0);
            return getChildren()[position];
                 
              
        }
        
        JSONPath operator [] (const Index& index)
        {
            assert(index > 0);
            getPositions()[index];
            return getChildren()[index];
        }
        
        void setType(Type type)
        {

            lock();
            Path path = *this;
    
            if (!path[TYPE].hasData())
            {
                if (!isRoot())
                    database().objects()[*this];
                path[TYPE].setData<Type>(type);
            }
            else if (JSONPath::type() != type)
            {
                
                clear();
                assert(isDeadEnd());
                if (!isRoot())
                    database().objects()[*this];
                path[TYPE].setData<Type>(type);
            }
            
            assert(JSONPath::type() == type);
            
        }
        
        void setNull()
        {
            setType(Type::NULL_);
        }
        
        void setUndefined()
        {
            setType(Type::UNDEFINED);
        }
        
        void setBoolean(const BString& value)
        {
            setType(Type::BOOLEAN);
            Path path = *this;
            path[VALUE].setData<BString>(value);
        }
        
        void setInteger(const BString& value)
        {
            setType(Type::INTEGER);
            Path path = *this;
            path[VALUE].setData<BString>(value);
        }
        
        void setNumber(const BString& value)
        {
            setType(Type::NUMBER);
            Path path = *this;
            path[VALUE].setData<BString>(value);
        }
        
        void setString(const BString& value)
        {
            setType(Type::STRING);
            Path path = *this;
            path[VALUE].setData<BString>(value);
            addWords(value, true);
                    
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
            
            Type type;
            path[TYPE].getData<Type>(type);
            
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
        
        static JSONPath fromString(JSONDatabase& database, const BString& host, const BeeFishWeb::URL& url, const BString& method = "GET")
        {
            JSONPath hostPath =
                 database.host(host);
            
            BString copy = url.path();
            char* str = copy.data();
            const char* delims ="/";
            char* token;

            // Get the first token
            token = strtok(str, delims);

            // Loop through all remaining tokens
            JSONPath path = hostPath;
            while (token != nullptr) 
            {
                BString key(token);

                if (key.size()) {
                    key = key.decodeURI();
                    if (key.isDigitsOnly()) {
                        Index index = atol(key.c_str());
                        if (path.contains(index))
                            path = path[index];
                        else
                            break;
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
                                ).unescape();
                        }
                        if (path.contains(key) || method == "POST")
                            path = path[key];
                        else
                            break;
                    }
                        


                }
                token = strtok(nullptr, delims);
            }
            
            if (token != nullptr) {
                throw PathNotFoundException(url);
            }
            
            return path;
        }
        
        bool contains(const BString& key)
        {
            if (type() != Type::OBJECT)
                return false;
            
            Path path =
                properties();
                 
            if (!path.contains(key))
            {
                return false;
            }
 
            path = path[key];
           
             
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
                 
            Path path = getPositions();
             
            if (!path.contains(index))
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
            
            if (objectPropertyPath.hasData())
            {
                objectPropertyPath.getData<Index>(position);
            }
            else 
            {
                // New property
                // Update the properties counter
                ++propertyPath;
                
                // Update positions
                Path positions = getPositions();
                position = ++positions;
                positions[position].setData<Index>(propertyPath.index());
                objectPropertyPath.setData<Index>(position);
                getChildren()[position];
                
                // add all path keys except the
                // first (host)
                BString key;

                JSONPath path = (*this)[property];
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
                        addWords(key, false);
                    }
    
                }
            }
            

            return position;
        }
    
        
        BString getObjectProperty(Index position)
        {
             
             Path path = getPositions();
             
             assert(path.contains(position));
    
             path = path[position];
             
             Index keyIndex = 0;
assert(path.hasData());
             path.getData<Index>(keyIndex);
             
             BString key;
             Path keyPath(database(), keyIndex);
             keyPath.parent(key);
             
             return key;
           
        }
        
        JSONPath operator [] (const char* key)
        {
            return (*this)[BString(key)];
        }
        
        friend ostream& operator << (ostream& out, const JSONPath& json)
        {
            JSONPath path(json);
            
            if (path.isDeadEnd())
                out << "undefined";
                 
            path.write(out, 0);
             
            return out;
        }
        
        BString tabs(Index tabCount) const
        {
            return string(tabCount * TAB_SPACES, ' ');
        }
        
        Path getPositions()
        {

            Path path = *this;
            
            path << POSITIONS;
            
            return path;
        }
        
        Path getChildren()
        {

            Path path = *this;
            
            path << CHILDREN;
            
            return path;
        }
        
        Path getObjectProperties()
        {
            setType(Type::OBJECT);
            Path path = *this;
            path << PROPERTIES;
            return path;
        }
        
        Index getKeyCount()
        {
            Path path = *this;
            
            if (type() != Type::OBJECT)
                return 0;
                
            Path objPositions = getPositions();
                 
            if (objPositions.isDeadEnd())
                return 0;
                     
            return objPositions.max<Index>();
        }
        
        Type type() const {
            Path path(*this);
            
            path << TYPE;
            
            if (!path.hasData())
                return Type::UNDEFINED;
                
            Type type;
            path.getData<Type>(type);
            return type;
        }
    
    public:
        
        virtual void clear() override
        {

            if (isDeadEnd())
                return;

            switch (type())
            {
                case Type::STRING:
                {
                    BString value;
                    Path path = *this;
                    path[VALUE].getData<BString>(value);
                    removeWords(value);
                }
                case Type::UNKNOWN:
                case Type::UNDEFINED:
                case Type::NULL_:
                case Type::BOOLEAN:
                case Type::INTEGER:
                case Type::NUMBER:
                    break;
                case Type::ARRAY:
                {

                    Path positions = getPositions();
                    Iterable<Index> iterable(positions);
                    
                    for (auto index : iterable)
                    {
                        JSONPath item = (*this)[index];
                        item.clear();
                        getPositions().clear(index);
                        getChildren().clear(index);
                    }

                    break;
                }
                case Type::OBJECT:
                {
                    Path positions = getPositions();
                    Iterable<Index> iterable(positions);
    
                    for (auto index : iterable)
                    {
                        BString property = getObjectProperty(index);
                        deleteProperty(property);
                    }
                    
                    break;
                    
                }
            }
                                
            assert(getPositions().isDeadEnd());
            assert(getChildren().isDeadEnd());
                    
            
            if (!isRoot())
                database().objects().clear(*this);
   
            Path::clear();
            assert(isDeadEnd());

        }
        
        void deleteProperty(const BString& property)
        {
    
            if (!contains(property))
                return;
                

            JSONPath json = (*this)[property];
        
            removeWords(property);

            // Remove parent properties
            JSONPath path = (*this)[property];
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
                    }
                    removeWords(property);
                }
                
            }
            
            
            // Remove the property
            Index position = getObjectPropertyPosition(property);
            json.clear();
            getPositions().clear(position);
            getChildren().clear(position);

            Path propertyPath = properties()[property];
            
            if (--propertyPath == 0) {
                properties().clear(property);
            }
            
            getObjectProperties().clear(propertyPath);

            
        }

        void addWords(const BString& word, bool addToParents)
        {

            Path words = database().words();
            Path objects = database().objects();
            Path object = objects[*this];
cerr << "+OBJECT " << word << " " << object.index() << endl;
   
            std::vector<BString> tokens =
                tokenise(word);
            
            
            for (auto token : tokens)
            {
                BString word =
                        token.toLower();
              
                Path wordPath = words[word];
cerr << "+WORDPATH " << word << " " << wordPath.index() << endl;
   
                JSONPath json = *this;
                if (addToParents) {
                    
                    while (!json.isRoot() &&
                        !json.parent().isRoot())
                    {
                        wordPath[json];
                        json = json.parent();
                    }
                }
                else {
                    wordPath[json];
                }
                object[wordPath];
                
                assert(object.contains(wordPath));
                
            }
        }
        
        void removeWords(const BString& value)
        {
            Path words = database().words();
            Path objects = database().objects();
            Path object = objects[*this];
cerr << "-OBJECT " << value << " " << object.index() << endl;
   
            std::vector<BString> tokens =
                tokenise(value);
            
            for (auto token : tokens)
            {
                BString word =
                        token.toLower();
                if (words.contains(word))
                {

                    Path wordPath = words[word];
cerr << "-WORDPATH " << word << " " << wordPath.index() << endl;
                    if (object.contains(wordPath))
                    {
cerr << "-OBJECTS[WORDPATH]" << endl;
                        object.clear(wordPath);
                        JSONPath json = (*this);
                        do
                        {
cerr << "-JSON " << json.index() << endl;
                            wordPath.clear(json);
                            if (wordPath.isDeadEnd())
                            {
cerr << "- " << word << endl;
                                words.clear(word);
                            }
                            json = json.parent();
                        }
                        while  (!json.isRoot() &&
                                !json.parent().isRoot());
                    }
                    
                }
            }
            
        }
        
        std::vector<BString> tokenise(BString word)
        {
            static const char* _deliminators =
                " \r\n\v\t()\".,!?{}+:; /";
        
            char* str = word.data();
            
            char* token = 
                strtok(str, _deliminators);
                
            std::vector<BString> words;

            // Loop through all remaining tokens
            while (token != nullptr) 
            {
                if (strlen(token))
                {
                    words.push_back(token);
                }
                token = strtok(nullptr, _deliminators); 
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
        
        
        virtual void write(ostream& out, Index tabCount = 0)
        {
            if (isDeadEnd())
                return;
          
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
                BString value;
                path[VALUE].getData<BString>(value);
                out << value;
                break;
            }
            case Type::STRING:
            {
                BString string;
                path[VALUE].getData<BString>(string);
                    
                out << "\""
                    << string.escape()
                    << "\"";
                 
                break;
            }
            case Type::ARRAY:
            {
                out << "[";
                 
                Index count = 0;
                 
                path = path[CHILDREN];
                
                Iterable<Index> arrayIndexes(path);
                
                if (!path.isDeadEnd())
                    count = path.max<Index>();
                 
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

                    item.write(
                        out,
                        _tabCount
                    );
                          
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
                Path objectPositions = getPositions();
                 
                if (objectPositions.isDeadEnd())
                {
                    out << "{}";
                    break;
                }
                     
                out << "{";
                Index count = 0;
                 
                if (!objectPositions.isDeadEnd()) {
                     
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
            Path _positions;
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
                    _positions = _jsonPath->getPositions();
                    _isEnd = !_positions.next<Index>(_stack, _position);
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
                    _positions.next<Index>(
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
                tmp._positions.save();
                ++(*this);
                tmp._positions.restore();
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
        Path path = json;
        return output << path;
    }
    
        
    
}

#endif
