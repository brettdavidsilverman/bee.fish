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
   db[OBJECTS][objectId][position] = keyIndex //[type][value]
   db[PROPERTIES][key][VALUE][type][value][objectId]
   db[PROPERTIES][key][POSITION][objectId] = position;
   //db[PROPERTIES][objectId][position] = keyIndex
   */
   
   
   
   class JSONPath :
      public MinMaxPath
   {
   protected:
      
      
   public:
      typedef Index Id;
      
      using MinMaxPath::contains;
      
      JSONPath()
      {
      }
      
      JSONPath(JSONDatabase& database, const Path& start) :
         MinMaxPath(start)
      {
         
      }
      
      JSONPath(Path start) :
         JSONPath((JSONDatabase&)start.database(), start)
      {
      }
      
      JSONPath(const JSONPath& source) :
         MinMaxPath(source)
      {
           
      }
      
      JSONDatabase& database()
      {
         return (JSONDatabase&)(*_database);
      }
      
      Id id()
      {
         return index();
      }
     
      Path properties()
      {
         return database().properties();
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
            
         Path path(*this);
        
         path << Type::OBJECT
              << position;
              
         return path;
          
      }
      
      JSONPath operator [] (const Index& index)
      {
         Path path(*this);
        
         path << Type::ARRAY
              << index;

         return path;
          
      }
      
      JSONPath operator [] (Type type)
      {
          Path path = *this;
          path << type;
          return path;
      }
      
      bool contains(const BString& key)
      {
         if (!contains(Type::OBJECT))
            return false;
            
         MinMaxPath path =
            properties()[BY_KEY];
            
         if (!path.contains(key))
         {
            return false;
         }
 
         path = path
            [key]
            [POSITIONS];
        
         
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
            properties()[BY_KEY];
            
         if (!keyPath.contains(key))
            keyPath[key].setData(key);
            
         
         keyPath = keyPath[key];
         
         Index keyPathIndex =
            keyPath.index();
         
         Path path = keyPath
            [POSITIONS]
            [id];
            
         Index position;
         
         if (path.hasData())
            path.getData(position);
         else {
               
            MinMaxPath object = (*this)[Type::OBJECT];
            
            if (object.isDeadEnd())
               position = 0;
            else {
               position =
                  object.max<Index>()
                  + 1;
            }

            path._setData<Index>(position);
            
         }
         
         properties()
            [BY_OBJECT]
            [id]
            [position]
            ._setData<Index>(keyPathIndex);
            
         return position;
      }
      
      // properties[key][POSITION][objectId]
      optional<BString> getObjectKey(Index position)
      {
         Path path = getObjectProperties();
         
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
      
      friend ostream& operator << (ostream& out, JSONPath& json)
      {
         json.write(out, 0);
         
         return out;
      }
      
      BString tabs(Size tabCount) const
      {
         return string(tabCount * TAB_SPACES, ' ');
      }
      
      Path getObjectProperties()
      {
         Path getObjectProperties =
            properties()
               [BY_OBJECT]
               [id()];
               
         return getObjectProperties;
      }
      
      virtual void clear()
      override
      {
               
         getObjectProperties().clear();
            
         Path::clear();
         
      }
      
      virtual void write(ostream& out, Size tabCount = 0)
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
                   << escape(string)
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
               JSONPath item = path[index];
                  
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
            Path objProps = getObjectProperties();
            
            if (objProps.isDeadEnd())
            {
               out << "{}";
               break;
            }
               
            out << "{";
            Size count = 0;
            
            
            if (!path.isDeadEnd()) {
               
               out << "\r\n";
              
               optional<BString> key;
               count = path.max<Size>() + 1;

               for (Size position = 0; position < count; ++position) 
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
                  
                     if (position < count - 1)
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
            throw std::logic_error("JSON::Variable::Value::copy constructor");
         }
          
      }
      
   };
   
   

}

#endif
