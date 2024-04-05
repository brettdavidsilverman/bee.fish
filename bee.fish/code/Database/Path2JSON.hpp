#ifndef BEE_FISH_JSON__PATH_2_JSON_HPP
#define BEE_FISH_JSON__PATH_2_JSON_HPP
#include "../Database/Path.hpp"
#include "../Database/MinMaxPath.hpp"
#include "../Script/Type.hpp"


namespace BeeFishJSON
{
    
   using namespace BeeFishDatabase;
   using namespace BeeFishScript;
   using namespace BeeFishMisc;
   
   class Path2JSON : public MinMaxPath
   {
   public:
       
      Path2JSON(const Path& start) :
         MinMaxPath(start)
      {
      }
       
      Path2JSON(Database& start) :
         Path2JSON(Path(start))
      {
      }
      
      friend ostream& operator << (ostream& out, Path2JSON& json)
      {
         json.write(out, 0);
         
         return out;
      }
      
      std::string tabs(Size tabCount)
      {
         return string(tabCount * TAB_SPACES, ' ');
      }
      
      virtual void write(ostream& out, Size tabCount)
      {
         if (isDeadEnd())
            return;
       
         Type type = 
            MinMaxPath(*this)
            .value<Type>();
            
         MinMaxPath path = (*this)[type];
            
         switch (type) {
         case Type::UNDEFINED:
            out << "undefined";
            break;
         case Type::NULL_:
            out << "null";
            break;
         case Type::BOOLEAN:
         {
            bool value = path.getData();
            if (value)
               out << "true";
            else
               out << "false";
            break;
         }
         case Type::INTEGER:
         {
            string number;
            path.getData(number);
            out << number;
            break;
         }
         case Type::NUMBER:
         {
            string number;
            path.getData(number);
            out << number;
            
            break;
         }
         case Type::STRING:
         {
            std::string string;
            path.getData(string);
              
            out << "\""
                   << escape(string)
                << "\"";
            
            break;
         }
         case Type::ARRAY:
         {
            out << "[";
            Size count = path.getData();
            
            if (count > 1) {
               out << "\r\n";
               out << tabs(tabCount + 1);
            }

            for (Size index = 0;
                 index < count;
                 ++index)
            {
               Path2JSON item =
                  path[index];
                  
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
               
               if (path.hasData())
                  count = path.getData();
                  
               Size i = 0;
               Stack stack;
               BeeFishScript::String key;
               MinMaxPath keys = path;
               while(keys.next<BeeFishScript::String>(stack, key))
               {
                  out << tabs(tabCount + 1)
                      << "\""
                         << escape(key)
                      << "\": ";
                      
                  Path2JSON value = path[key];
                  
                  value.write(out, tabCount + 1);
                  
                  if (++i < count)
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