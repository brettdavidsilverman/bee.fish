#ifndef BEE_FISH_JSON__PATH_2_JSON_HPP
#define BEE_FISH_JSON__PATH_2_JSON_HPP
#include "../b-string/string.h"
#include "../Database/Path.hpp"
#include "../Database/MinMaxPath.hpp"
#include "../Script/Type.hpp"
#include "JSONIndex.hpp"

namespace BeeFishJSON
{
    
   using namespace BeeFishDatabase;
   using namespace BeeFishScript;
   using namespace BeeFishMisc;
   
   class Path2JSON : public MinMaxPath
   {
   protected:
      MinMaxPath _properties;
   public:
       
      Path2JSON(const Path& properties, const Path& objects) :
         MinMaxPath(objects),
         _properties(properties)
      {
      }
       
      Path2JSON( Database& database) :
         Path2JSON(
            Path(database)[PROPERTIES],
            Path(database)[OBJECTS]
         )
      {
      }
      
      friend ostream& operator << (ostream& out, Path2JSON& json)
      {
         json.write(out, 0);
         
         return out;
      }
      
      BString tabs(Size tabCount)
      {
         return string(tabCount * TAB_SPACES, ' ');
      }
      
      virtual void write(ostream& out, Size tabCount)
      {
         if (isDeadEnd())
            return;
       
         Type type = value<Type>();
            
         MinMaxPath path = (*this)[type];
            
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
               Path2JSON item(
                  _properties,
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
               
               Stack stack(*this);
               BString key;
               Index keyIndex;
               MinMaxPath keyIndexes = path;
               count = path.max<Size>() + 1;
               for (Size index = 0; index < count; ++index) 
               {
                  path[index].getData(keyIndex);
                  Path keyPath(path, keyIndex);
                  keyPath.getData(key);

                  out << tabs(tabCount + 1)
                      << "\""
                         << escape(key)
                      << "\": ";
                      
                  Path2JSON value(_properties, path[index]);
                  
                  value.write(out, tabCount + 1);
                  
                  if (index < count - 1)
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