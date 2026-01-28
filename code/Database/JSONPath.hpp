#ifndef BEE_FISH_DATABASE_JSON_PATH_HPP
#define BEE_FISH_DATABASE_JSON_PATH_HPP
#include "../Database/DatabaseBase.hpp"
#include "../Database/Path.hpp"
#include "../json/json.h"
#include "../json/json-parser.h"
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
        /*
        template<typename T>
        JSONPath operator [] (const T& key)
        {

             return Path::operator[](key);
        }
        
        */
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
                 
                    
             return (*this)
                 [Type::OBJECT]
                 [position];
              
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
        
        JSONPath parent(BString& key) {
            JSONPath path = *this;

            Index position = 0;
            path = path.parent(position);
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
                key = path.getObjectKey(position).value();
            }
    
            return path;
            
        }
        
        BString toString() {
            vector<BString> keys;
            JSONPath path = *this;
            JSONPath root = database().objects();
            while (path != root && path.index())
            {
                BString key;
                path = path.parent(key);
                keys.push_back(key);
            }
            
            std::reverse(keys.begin(), keys.end());
            
            BString string = database().origin();
            for (auto key : keys)
            {
                string += "/" + key;
            }
            
            return string;
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
             if (!contains(Type::ARRAY))
                 return false;
                 
             Path path = (*this)[Type::ARRAY];
             
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
                 
             if (!keyPath.contains(key))
                 keyPath[key].setData(key);
                 
             
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
             keyPath.getData(key);
             
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
        
        Type type() const {
            Path path(*this);
            return path.max<Type>();
        }
        
        
        virtual void clear()
        override
        {
                     
           // getObjectPositions().clear();
                 
             Path::clear();
             
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
                           << escape(string)
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
                     out << "\r\n";
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
                          out << ",\r\n" << tabs(tabCount + 1);
                     }
                 }

                 if (count > 1) {
                     out << "\r\n";
                     out << tabs(tabCount);
                 }
                 
                 out << "]";
                     
                 break;
             }
             case Type::OBJECT:
             {
                 Path objProps = getObjectPositions();
                 
                 if (objProps.isDeadEnd())
                 {
                     out << "{}";
                     break;
                 }
                     
                 out << "{";
                 Index count = 0;
                 
                 
                 if (!path.isDeadEnd()) {
                     
                     out << "\r\n";
                    
                     optional<BString> key;
                     count = path.max<Index>();

                     for (Index position = 1; position <= count; ++position) 
                     {
                          key = getObjectKey(position);

                          if (key.has_value()) {
                              out << tabs(tabCount + 1)
                                    << "\""
                                        << escape(key.value())
                                    << "\": ";
                               
                              JSONPath value =
                                  path[position];
                          
                              value.write(out, tabCount + 1);
                          
                              if (position < count)
                                  out << ",\r\n";
                          }

                     }
                 }
                 
                 if (count > 0) {
                     out << "\r\n";
                     out << tabs(tabCount);
                 }
                 
                 out  << "}";
                 break;
             }
             default:
                 throw std::logic_error("JSONPath::write");
             }
            
        }
        
    };
    
    

}

#endif
