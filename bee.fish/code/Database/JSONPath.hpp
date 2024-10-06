#ifndef BEE_FISH_DATABASE_JSON_PATH_HPP
#define BEE_FISH_DATABASE_JSON_PATH_HPP
#include "../Database/DatabaseBase.hpp"
#include "../json/json.h"
#include "../json/json-parser.h"
#include "JSONIndex.hpp"

namespace BeeFishDatabase {
 
   using namespace BeeFishParser;
   using namespace BeeFishScript;
   using namespace BeeFishJSON;

   /*
   db[OBJECTS][objectId][position][type][value]
   db[PROPERTIES][key][VALUE][type][value][objectId]
   db[PROPERTIES][key][POSITION][objectId] = position;
   db[PROPERTIES][objectId][position] = key
   */
   
   class JSONPath :
      public MinMaxPath
   {
   protected:
      Path _properties;
      Path _objects;
      
   public:
      typedef Index Id;
      
      JSONPath()
      {
      }
      
      JSONPath(Database& database, const Path& start) :
         MinMaxPath(start)
      {
         Path root(database);
         
         _properties =
            root[HOST][PROPERTIES];
            
         _objects =
            root[HOST][OBJECTS];
           
      }
      
      JSONPath(Path start) :
         JSONPath(start.database(), start)
      {
      }
      
      JSONPath(const JSONPath& source) :
         MinMaxPath(source),
         _properties(source._properties),
         _objects(source._objects)
      {
           
      }
      
      Id id()
      {
         //Path path = *this;
        // Index index = path[type].index();
         return index();
      }
      
      template<typename T>
      JSONPath operator [] (const T& key)
      {

         return MinMaxPath::operator[](key);
      }

      JSONPath operator [] (const BString& key)
      {
        
         Index position =
            getObjectKeyPosition(key);
            
         JSONPath path(*this);
        
         path //<< Type::OBJECT
              << position;
              
         return path;
          
      }
      
      JSONPath operator [] (Index arrayIndex)
      {
       
         JSONPath path = *this;
         
         path //<< Type::ARRAY
              << arrayIndex;

         return path;
          
      }
      
      JSONPath operator [] (Type type)
      {
          JSONPath path = *this;
          path << (Size)type;
          return path;
      }
      
      bool contains(const BString& key)
      {
         Path keyPath =
            _properties[BY_KEY];
            
         if (!keyPath.contains(key))
            return false;
 
         Path path = keyPath
            [key]
            [POSITIONS];
         
         return path.contains(
            id()
         );
      }
      
      
      // properties[key][POSITION][objectId]
      Index getObjectKeyPosition(const BString& key)
      {
         Id id = this->id();
         
         Path keyPath =
            _properties[BY_KEY][key];
 
         keyPath.setData(key);
         
         Path path = keyPath
            [POSITIONS]
            [id];
            
         Index position;
         
         if (path.hasData())
            path.getData<Index>(position);
         else {
               
            MinMaxPath object = (*this);
            
            if (object.isDeadEnd())
               position = 0;
            else {
               position =
                  object.max<Index>()
                  + 1;
            }

            path.setData<Index>(position);
            
         }
         
         _properties
            [BY_OBJECT]
            [id]
            [position]
            .setData<Index>(keyPath.index());

         return position;
      }
      
      // properties[key][POSITION][objectId]
      BString getObjectKey(Index position)
      {
         Id id = (*this)[Type::OBJECT].id();

         Path path = _properties
            [BY_OBJECT]
            [id]
            [position];
            
         assert(path.hasData());
         
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
      
      friend ostream& operator << (ostream& out, JSONPath& json)
      {
         json.write(out, 0);
         
         return out;
      }
      
      BString tabs(Size tabCount) const
      {
         return string(tabCount * TAB_SPACES, ' ');
      }
      
      virtual void write(ostream& out, Size tabCount)
      {
         if (isDeadEnd())
            return;
       
         Type type = max<Type>();
         
         
         MinMaxPath path =
            Path(*this)[type];
            
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
            
            Size count = 0;
            
            if (!path.isDeadEnd())
               count = path.max<Size>() + 1;
            
            if (count > 1) {
               out << "\r\n";
               out << tabs(tabCount + 1);
            }

            for (Size index = 0;
                 index < count;
                 ++index)
            {
               JSONPath item(
                  path[index]
               );
                  
               Size _tabCount = tabCount + 1;
                     
               if (count == 1)
                  _tabCount--;

               item.write(
                  out,
                  _tabCount
               );
                  
               if (index < count - 1)
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
            
            out << "{";
            Size count = 0;

            if (!path.isDeadEnd()) {
               
               out << "\r\n";
               
               BString key;
               count = path.max<Size>() + 1;
               for (Size position = 0; position < count; ++position) 
               {
                  key = getObjectKey(position);

                  out << tabs(tabCount + 1)
                      << "\""
                         << escape(key)
                      << "\": ";
                      
                  JSONPath value(path[position]);
                  
                  value.write(out, tabCount + 1);
                  
                  if (position < count - 1)
                     out << ",\r\n";

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
            throw std::logic_error("JSON::Variable::Value::copy constructor");
         }
          
      }
      
   };
   
   

}

#endif
