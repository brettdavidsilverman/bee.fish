#ifndef BEE_FISH_JSON__PATH_2_JSON_HPP
#define BEE_FISH_JSON__PATH_2_JSON_HPP
#include "../Database/Path.hpp"
#include "../Database/MinMaxPath.hpp"
#include "../Script/Type.hpp"


namespace BeeFishJSON
{
    
   using namespace BeeFishDatabase;
   using namespace BeeFishScript;

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
         case Type::NULL_:
         case Type::BOOLEAN:
            out << path.value<BeeFishScript::String>();
            break;
         case Type::INTEGER:
            out << path.value<Integer>();
            break;
         case Type::NUMBER:
         {
            BeeFishScript::String number =
               (BeeFishScript::String)path;
               
            out << number;
            
            break;
         }
         case Type::STRING:
         {
            out << "\"" << escape(
               path.value<BeeFishScript::String>()
            ) << "\"";
            break;
         }
         case Type::ARRAY:
         {
            out << "[";
            Size max = 0;
            
            if (!path.isDeadEnd()) {
               Stack stack;
               Size index;
               
               max = path.max<Size>();
               
               if (max > 0) {
                  out << "\r\n";
                  out << tabs(tabCount + 1);
               }
               
               for (index = path.min<Size>(stack);
                    index <= max;
                    ++index)
               {
                  if (path.contains(index))
                  {
                     Path2JSON item =
                        path[index];
                  
                     item.write(
                        out,
                        tabCount
                     );
                  }
                  
                  if (index < max)
                  {
                     out << ",\r\n" << tabs(tabCount + 1);
                  }
               }
            }
            
            if (max > 0) {
               out << "\r\n";
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
                 // cerr << "PATH2JSON: " << key << endl;
                  
                  out << tabs(tabCount + 1)
                      << "\""
                         << escapeString(key)
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