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
    public:
        
        
        inline static const Index PROPERTIES = 0;
        inline static const Index POSITIONS = 1;
        
    public:
        typedef Index Id;
        
        using Path::contains;
        
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
                 getObjectPropertyPosition(property);
                 
            Path path =
                 properties()[property];
                 
            Index propertyIndex = path.index();
            
            getPositions().clearValue(position);
            getObjectProperties().clearValue(propertyIndex);
        }

        JSONPath operator [] (const BString& key)
        {
             Index position =
                 getObjectPropertyPosition(key);
                 
             Path json = getPositions()[position];
                 
             return json;
              
        }
        
        JSONPath operator [] (const Index& index)
        {
            JSONPath path(*this);
            Type type = path.type();
            path << type << POSITIONS << index;
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
        
        JSONPath parent(BString& key) {
            Path path = *this;

            Index position = -1;
            
            assert (!path.isRoot());
            //    return false;
            
            path = path.parent(position);
            
            assert(!path.isRoot());
            
            Index seperator;
            path = path.parent(seperator);
            assert(seperator == POSITIONS);
            
            assert(!path.isRoot());
            
            Type type = Type::UNKNOWN;
            
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
                key = object.getObjectProperty(position).value();
                if (key.isDigitsOnly())
                {
                    key = BString("\"") + key + BString("\"");
                }
                else
                    key = key.escape();
            }
    
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
                 getObjectProperties()
                 .contains(path.index());
             
             return contains;
             
        }
        
        bool contains(const Index& index)
        {
             if (!contains(Type::ARRAY) &&
                 !contains(Type::OBJECT))
                 return false;
                 
             Path path = getPositions();
             
             if (!path.contains(index))
             {
                 return false;
             }
             
 
             return true;
             
        }
        /*
        void lock()
        {
            database().lock();
        }
        
        void unlock()
        {
            database().unlock();
        }
        
        */
        Index getObjectPropertyPosition(const BString& key)
        {
            // Get the property index
            Path keyPath = 
                properties();
                 
            keyPath = keyPath[key];
             
            Index keyPathIndex =
                keyPath.index();
             
             
            // Get the position
            Index position;
            
            Path objectKeyPath = getObjectProperties()[keyPathIndex];
            lock();
            if (objectKeyPath.hasData())
            {
                objectKeyPath.getData<Index>(position);
            }
            else 
            {
                
                Path objectPositions = getPositions();
                
                if (objectPositions.isDeadEnd())
                    position = 1;
                else {
                    position =
                        objectPositions.max<Index>()
                        + 1;
                }
                objectPositions[position].setData<Index>(keyPathIndex);
            
                objectKeyPath.setData<Index>(position);
            
            }
            unlock();
            
            return position;
        }
    
        
        optional<BString> getObjectProperty(Index position)
        {
             Path path = getPositions();
             
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
            return (*this)
                [type()]
                [POSITIONS];
        }
        
        Path getObjectProperties()
        {
            return  (*this)
                 [Type::OBJECT]
                 [PROPERTIES];
        }
        
        Index getKeyCount()
        {
            if (!contains(Type::OBJECT))
                return 0;
                
            Path objPositions = getPositions();
                 
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
            Index position = getObjectPropertyPosition(key);
            getPositions().clearValue(position);
            

            Path keyPath = properties()[key];
            getObjectProperties().clearValue(keyPath.index());
            
             
        }

    public:
        void addWords(const BString& word, bool addToParents)
        {
   
            Path words = database().words();
            
            std::vector<BString> tokens =
                tokenise(word);
            
            for (auto token : tokens)
            {
                BString word =
                        token.toLower();
                
                
                if (addToParents) {
                    JSONPath path = *this;
                    while (!path.isRoot() &&
                        !path.parent().isRoot())
                    {
                        words[word][path.id()];
                        path = path.parent();
                    }
                }
                else
                    words[word][id()];
                
            }
        }
        
        void removeWords(const BString& word)
        {
   
            Path words = database().words();
        
            std::vector<BString> tokens =
                tokenise(word);
            
            for (auto token : tokens)
            {
                BString word =
                        token.toLower();
        
                if (words.contains(word))
                {
                    Path wordPath = words[word];
                    JSONPath path = *this;
                    while (!path.isRoot() &&
                           !path.parent().isRoot())
                    {
                        wordPath.clearValue(path.id());
                        if (wordPath.isDeadEnd()) {
                            words.clearValue(word);
                        }
                        path = path.parent();
                    }
                    
                    
                    
                }
            }
        }
        
        std::vector<BString> tokenise(BString word)
        {
            static const char* _whiteSpace =
                " \r\n\v\t";
                
            static const char* _deliminators =
                "-+ .,!?()/\"\'{}\\";
        
            char* str = word.data();
            char* token = 
                strtok(str, _whiteSpace);
            std::vector<BString> words;
            std::vector<BString> outer;
            
            // Loop through all remaining tokens
            while (token != nullptr) 
            {
                if (strlen(token))
                {
                    words.push_back(token);
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
                    if (strlen(token)) {
                        
                        words.push_back(token);
                    }
                    
                    token = strtok(nullptr, _deliminators); 
                }
                
            }
            
            return words;
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
                 
                path = path[POSITIONS];
                
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
                        _item = _jsonPath->getObjectProperty(_position).value();
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
                    _item = _jsonPath->getObjectProperty(_position).value();
                    
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
        

    
    
        
    
}

#endif
