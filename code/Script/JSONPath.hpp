#ifndef BEE_FISH__SCRIPT__JSON_PATH
#define BEE_FISH__SCRIPT__JSON_PATH

#include "../Database/Path.hpp"
#include "../Script/Variable.hpp"

using namespace BeeFishDatabase;
using namespace BeeFishScript;
using namespace BeeFishJSON;

namespace BeeFishScript {
   
   typedef 
      BeeFishDatabase::
      Path<BeeFishPowerEncoding::PowerEncoding>
      Path;
  
   class JSONPath :
      public Path
   {
   public:
      JSONPath(const Path& source) :
         Path(source)
      {
      }

      void setVariable(const Variable& source)
      {
         Path path(*this);
         path << source._type;
         string value;
         switch (source._type)
         {
            case Type::UNDEFINED:
               path.setData("undefined");
               break;
            case Type::_NULL:
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
                  Variable element = (*array)[i];
                  JSONPath(path[i]).setVariable(element);
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
               
               Path table = path[OBJECT_TABLE];
               Path map = path[OBJECT_KEYS];
               Size i = 0;
               for (auto it = object->cbegin(); it != object->cend(); ++it)
               {
                  
                  String key = *it;
                  table[i++].setData(key);
                  Variable item = (*object)[key];
                  JSONPath(map[key])
                     .setVariable(item);
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
         Path path(*this);

         if (path.isDeadEnd())
            return undefined;

         Type type =
            path.value<Type>();
         path << type;
         Variable var(type);
         string value;
         path.getData(value);
         switch (type)
         {
            case Type::UNDEFINED:
               break;
            case Type::_NULL:
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
               for (Size i = 0; i <= max; ++i)
               {
                  JSONPath value = path[i];
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

               Path table = path[OBJECT_TABLE];
               Path map = path[OBJECT_KEYS];


               Path::Stack stack;
               String key;
               Size i;
               while (table.next(stack, i))
               {
                  table[i].getData(key);
                  JSONPath json = map[key];
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

      JSONPath operator [] (const string& key)
      {
         Path path(*this);

         Type type =
            path.value<Type>();

         path << type;

         path << OBJECT_KEYS << key;
         
         return JSONPath(path);
      }

      JSONPath operator [] (const char * str)
      {
         string key(str);

         return (*this)[key];
      }

      JSONPath operator [] (const Size& index)
      {
         Path path(*this);

         Type type =
            path.value<Type>();

         path << type;

         if (type == Type::OBJECT)
         {
            Path table = path[OBJECT_TABLE];
            String key;
            table.getData(key);
            path << OBJECT_KEYS
                 << key;
         }
         else //if (type == Type::ARRAY)
            path << index;
         
         
         return JSONPath(path);
      }

      JSONPath operator [] (int index)
      {
         return (*this)[(Size)index];
      }
      
      bool contains(std::string& segment)
      {
         Path path(*this);

         Type type =
            path.value<Type>();

         if (type == Type::OBJECT)
         {
            path << type;
            path << OBJECT_KEYS;
            if (path.contains(segment))
               return true;
         }
         
         return false;
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