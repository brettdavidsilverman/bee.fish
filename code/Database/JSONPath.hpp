#ifndef BEE_FISH_DATABASE_JSON_PATH_HPP
#define BEE_FISH_DATABASE_JSON_PATH_HPP
#include "../json/json.h"
#include "../json/json-parser.h"
#include "../Database/DatabaseBase.hpp"
#include "../Database/Path.hpp"
#include "../web-request/web-request.h"

#include "JSONIndex.hpp"

namespace BeeFishDatabase {
 
    using namespace BeeFishParser;
    using namespace BeeFishScript;
    using namespace BeeFishJSON;


    
    class JSONPath :
        public Path
    {
    protected:
        
        
    public:
        typedef Index Id;
        
        using Path::contains;
        using Path::parent;
        
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

        void clearValue(const BString& property)
        {
            
            Index position =
                 getObjectKeyPosition(property);
                 
            Path path =
                 properties();
                 
            path = path[property];
           
            path.clearValue(id());
            
            Path objectProperties = getObjectPositions();
            objectProperties.clearValue(position);
        }

        JSONPath operator [] (const BString& key)
        {
             Index position =
                 getObjectKeyPosition(key);
                 
             Path json = (*this)[Type::OBJECT][position];
                 
             return json;
              
        }
        
        JSONPath operator [] (const Index& index)
        {
             JSONPath path(*this);
             path << path.type() << index;
             return JSONPath(path);
        }
        
        Path operator [] (Type type)
        {
              Path path = *this;
              path << type;
              return path;
        }
        
        JSONPath parent() {
            BString key;
            return parent(key);
        }
        
        bool parent(JSONPath& parent, BString& key) {
            Path path = *this;

            Index position = -1;
            
            if (path.isRoot())
                return false;
            
            path = path.parent(position);
            
            if (path.isRoot())
                return false;
            
            Type type;
            path = path.parent(type);

            if (type == Type::ARRAY)
            {
                stringstream stream;
                stream << position;
                key = stream.str();
            }
            else if (type == Type::OBJECT)
            {
                JSONPath object = path;
                key = object.getObjectKey(position).value();
                if (key.isDigitsOnly())
                {
                    key = BString("\"") + key + BString("\"");
                }
                else
                    key = key.escape();
            }
    
            parent = path;
            
            return true;
            
        }
        
        
        
        BString toString() {
            JSONPath path = *this;
            JSONPath parent;
            BString string;
            BString key;
            while (path.parent(parent, key))
            {
                BString newString =
                    key + BString("/") + string;
                string = newString;
                path = parent;
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
             if (!contains(Type::OBJECT))
                 return false;
                 
             Path path =
                 properties();
                 
             if (!path.contains(key))
             {
                 return false;
             }
 
             path = path[key];
           
             
             bool contains =
                 path.contains(id());
             
             return contains;
             
        }
        
        bool contains(const Index& index)
        {
             if (!contains(Type::ARRAY) &&
                 !contains(Type::OBJECT))
                 return false;
                 
             Path path = (*this)[type()];
             
             if (!path.contains(index))
             {
                 return false;
             }
             
 
             return true;
             
        }
        
        
        // properties[key][POSITION][objectId]
        Index getObjectKeyPosition(const BString& key)
        {
             Id id = this->id();
             Path keyPath = 
                 properties();
                 
             keyPath = keyPath[key];
             
             Index keyPathIndex =
                 keyPath.index();
             
             Path path = keyPath[id];
                 
             Index position;
             
             if (path.hasData())
                 path.getData(position);
             else {
                     
                 Path object = (*this)[Type::OBJECT];
                 
                 if (object.isDeadEnd())
                     position = 1;
                 else {
                     position =
                          object.max<Index>()
                          + 1;
                 }

                 path.setData<Index>(position);
                 
             }
             
             getObjectPositions()
                 [position]
                 .setData<Index>(keyPathIndex);

             return position;
        }
        
        // properties[key][POSITION][objectId]
        optional<BString> getObjectKey(Index position)
        {
             Path path = getObjectPositions();
             
             if (!path.contains(position))
                 return nullopt;
                 
             path = path[position];
             
             Index keyIndex = 0;
             path.getData<Index>(keyIndex);
             BString key;
             Path keyPath(database(), keyIndex);
             keyPath.parent(key);
             
             return key;
           
        }
        
        //_properties[key][VALUE][type][value][objectId]
        
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
        
        Path getObjectPositions()
        {
            Path path(*this);
            return path[Type::OBJECT];
        }
        
        Index getKeyCount()
        {
            if (!contains(Type::OBJECT))
                return 0;
                
            Path objPositions = getObjectPositions();
                 
            if (objPositions.isDeadEnd())
                return 0;
                     
            return objPositions.max<Index>();
        }
        
        Type type() const {
            Path path(*this);
            return path.max<Type>();
        }
        
        
        void deleteKey(const BString& key)
        {
            Index position = getObjectKeyPosition(key);
            getObjectPositions().clearValue(position);
            
            Id id = this->id();

            Path keyPath = properties()[key];
            keyPath.clearValue(id);
            if (keyPath.isDeadEnd())
                properties().clearValue(key);
            
             
        }
        
        virtual void write(ostream& out, Index tabCount = 0)
        {
            if (isDeadEnd())
                return;
          
            Type type = JSONPath::type();
             
            Path path =
                (*this)[type];
                 
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
                path.getData(value);
                out << value;
                break;
            }
            case Type::STRING:
            {
                BString string;
                path.getData(string);
                    
                out << "\""
                    << string.escape()
                    << "\"";
                 
                break;
            }
            case Type::ARRAY:
            {
                out << "[";
                 
                Index count = 0;
                 
                if (!path.isDeadEnd())
                    count = path.max<Index>();
                 
                if (count > 1) {
                    out << "\n";
                    out << tabs(tabCount + 1);
                }

                for (Index index = 1;
                    index <= count;
                    ++index)
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
                Path objectPositions = getObjectPositions();
                 
                if (objectPositions.isDeadEnd())
                {
                    out << "{}";
                    break;
                }
                     
                out << "{";
                Index count = 0;
                 
                if (!path.isDeadEnd()) {
                     
                    out << "\n";
                    
                    BString key;
                    for (auto iterator = begin(); iterator != end(); ++iterator)
                    {
                        key = *iterator;

                        ++count;
                        
                        out << tabs(tabCount + 1)
                            << "\""
                                << key.escape()
                            << "\": ";
                               
                        JSONPath value =
                            path[iterator.position()];
                          
                        value.write(out, tabCount + 1);
                          
                        auto iteratorCopy = iterator;
                        if (++iteratorCopy != end())
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
        
        class PathIterator {
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

            PathIterator(JSONPath* path, bool isEnd = false)
                : _jsonPath(path)
            {
                
                if (isEnd) {
                    _isEnd = true;
                }
                else {
                    _positions = _jsonPath->getObjectPositions();
                    _isEnd = !_positions.next<Index>(_stack, _position);
                    if (!_isEnd) {
                        _item = _jsonPath->getObjectKey(_position).value();
                    }
                }
            }
            
            
            // Dereference operator (*)
            reference operator*() const
            {
                return _item;
            }

            // Prefix increment operator (++)
            PathIterator& operator++() {
        
                _isEnd = _isEnd or not
                    _positions.next<Index>(
                        _stack,
                        _position
                    );
                    
                if (!_isEnd)
                    _item = _jsonPath->getObjectKey(_position).value();
                    
                return *this;
            }

            // Postfix increment operator (++)
            PathIterator operator++(int) {
                PathIterator tmp = *this;
                ++(*this);
                return tmp;
            }

            friend bool operator == (
                const PathIterator& a,
                const PathIterator& b
            )
            {
                return  (a._isEnd == b._isEnd &&
                         a._jsonPath == b._jsonPath &&
                         a._stack == b._stack);
            }
            
            friend bool operator != (
                const PathIterator& a,
                const PathIterator& b
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
        PathIterator begin() {
            return PathIterator(this);
        }
    
        // Points one past the last element
        PathIterator end() { 
            PathIterator iterator(this, true);
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
        

    
    
        
    
}

#endif
