#ifndef BEE_FISH__SCRIPT__JSON_PATH
#define BEE_FISH__SCRIPT__JSON_PATH

#include "../Database/MinMaxPath.hpp"
#include "../Script/Variable.hpp"

using namespace BeeFishDatabase;
using namespace BeeFishScript;

namespace BeeFishDBServer {
   

   class JSONPath :
      public MinMaxPath
   {
   public:

      JSONPath(const Path& source) :
         MinMaxPath(source)
      {
      }
      /*
      const Variable& operator=(const Variable& var)
      {
          setVariable(var);
          return var;
      }
      
      operator Variable() {
         Variable var = getVariable();
         return var;
      }
      
*/

      JSONPath operator [] (Size index) {
         Path path = *this;
         path = path[Type::ARRAY][index];
         return path;
      }
      
      JSONPath operator [] (String key) {
         Path path = *this;
         path = path[Type::OBJECT][key];
         return path;
      }
      
      JSONPath operator [] (const char* key) {
         Path path = *this;
         path = path[Type::OBJECT][key];
         return path;
      }
      
      bool contains(const String& key) const {
          Path path = *this;
          return path.contains(Type::OBJECT) &&
                 path[Type::OBJECT].contains(key);
      }
          
    
      
      void setVariable(const Variable& source)
      {
         Path path(*this);
         path = path[source._type];
         string value;
         switch (source._type)
         {
            case Type::UNDEFINED:
               path.setData("undefined");
               break;
            case Type::NULL_:
               path.setData("null");
               break;
            case Type::BOOLEAN:
               if (source._value._boolean)
                  path.setData("true");
               else
                  path.setData("false");
               break;
            case Type::NUMBER:
            {
               stringstream stream;
               stream << source._value._number;
               path.setData(stream.str());
               break;
            }
            case Type::STRING:
               path.setData(source._value._string);
               break;
            case Type::ARRAY:
            {
               ArrayPointer array (source._value._array);
               Size i;
               for (i = 0; i < array->size(); ++i)
               {
 
                  Variable& element = (*array)[i];
                  JSONPath indexed = (*this)[i];
                  indexed.setVariable(element);
               }
               stringstream stream;
               stream << i;
               path.setData(stream.str());
               break;
            }
            case Type::OBJECT:
            {
                
               ObjectPointer object =
                  (source._value._object);
                  
               Size i = 0;
               for (auto pair : *object)
               {
                  
                  const String& key = pair.first;
                  const Variable& item = pair.second;
  
                  JSONPath indexed = path[key];
                  indexed.setVariable(item);
                  ++i;
               }
               stringstream stream;
               stream << i;
               path.setData(stream.str());

               break;
            }
            default:
               throw std::logic_error("JSON::Variable::Value::copy constructor");
         }
         
      }

      Variable getVariable() {
         MinMaxPath path(*this);

         if (path.isDeadEnd())
            return undefined;

         Type type =
            path.value<Type>();
         path = path[type];
         Variable var(type);
         string value;
         path.getData(value);
         switch (type)
         {
            case Type::UNDEFINED:
               break;
            case Type::NULL_:
               break;
            case Type::BOOLEAN:
               if (value == "true")
                  var._value._boolean = true;
               else
                  var._value._boolean = false;
               break;
            case Type::NUMBER:
            {
               stringstream stream;
               stream << value;
               stream >> var._value._number;
               break;
            }
            case Type::STRING:
               var._value._string = value;
               break;
            case Type::ARRAY:
            {
               ArrayPointer array = ArrayPointer(new Array());
               var._value._array = array;
               Size max = path.max<Size>();
               array->reserve(max);
               JSONPath json = path;
               for (Size i = 0; i <= max; ++i)
               {
                  JSONPath value = json[i];
                  array->push_back(
                     value.getVariable()
                  );
               }

               break;
            }
            case Type::OBJECT:
            {
               BeeFishScript::ObjectPointer
                  object = BeeFishScript::ObjectPointer(
                     new BeeFishScript::Object()
                  );
                  
               var._value._object = object;

               Stack stack;
               String key;
               while (path.next(stack, key))
               {
                  JSONPath json = path[key];
                  Variable value = json.getVariable();
                  (*object)[key] = value;
               }
               
               break;
            }
            default:
               throw std::logic_error("JSON::Variable::Value::copy constructor");
         }
         return var;
      }
      
      friend ostream& operator << (ostream& out, const JSONPath& jsonPath)
      {
         Path path = jsonPath;
         out << path;
         return out;
      }


   };

}

#endif