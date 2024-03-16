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
      
      virtual void write(ostream& out, Size tabs)
      {
         if (isDeadEnd())
            return;
     
         Stack stack;
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
            Number number = path.getData();
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
            if (!path.isDeadEnd()) {
               Stack stack;
               Size index;
               
               Size max = path.max<Size>();
               cerr << "MAX: " << max << endl;
               for (index = path.min<Size>(stack);
                    index <= max;
                    ++index)
               {
                  if (path.contains(index)) {
                      Path2JSON item =
                         path[index];
                     item.write(out, tabs + 1);
                  }
                     
                  if (index < max)
                     out << ",";
               }
            }
            out << "]";
            break;
         }
         case Type::OBJECT:
            out << "{Not implemented}" << endl;
            break;
         default:
            throw std::logic_error("JSON::Variable::Value::copy constructor");
         }
          
      }
       
    };
}

#endif