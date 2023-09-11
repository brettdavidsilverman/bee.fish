#ifndef BEE_FISH__DBSERVER__JSON_PATH
#define BEE_FISH__DBSERVER__JSON_PATH

#include "../Database/Path.hpp"
#include "Variable.hpp"

using namespace BeeFishDatabase;
using namespace BeeFishScript;

namespace BeeFishDBServer {

   class JSONPath : protected BeeFishWeb::Path
   {
   public:
      const int DATA = 0;
      const int OBJECT_TABLE = 0;
      const int OBJECT_VALUES = 1;

   public:
      JSONPath(const Path& source) :
         Path(source)
      {
      }

      void setVariable(const Variable& source)
      {
         setData(source._type);
         Path path(*this);
         switch (source._type) {
            case BeeFishJSON::Type::UNDEFINED:
               break;
            case BeeFishJSON::Type::_NULL:
               break;
            case BeeFishJSON::Type::BOOLEAN:
               path[DATA].setData(source._value._boolean);
               break;
            case BeeFishJSON::Type::NUMBER:
               path[DATA].setData(source._value._number);
               break;
            case BeeFishJSON::Type::STRING:
               path[DATA].setData(source._value._string);
               break;
            case BeeFishJSON::Type::ARRAY:
            {
               ArrayPointer array (source._value._array);
               for (Size i = 0; i < array->size(); ++i)
               {
                  Variable element = (*array)[i];
                  (*this)[i].setVariable(element);
               }
               break;
            }
            case BeeFishJSON::Type::OBJECT:
            {
               ObjectPointer object =
                  (source._value._object);
               
               Path table = path[OBJECT_TABLE];
               Path map = path[OBJECT_VALUES];
               Size i = 0;
               for (auto it = object->cbegin(); it != object->cend(); ++it)
               {
                  
                  String key = *it;
                  table[i].setData(key);
                  Variable item = (*object)[key];
                  JSONPath(map[key])
                     .setVariable(item);
                  ++i;
               }

               break;
            }
            default:
               throw std::logic_error("JSON::Variable::Value::copy constructor");
         }
         
      }

      Variable getVariable() {
         BeeFishJSON::Type type;
         getData(type);
         Variable var(type);
         Path path(*this);
         switch (type) {
            case BeeFishJSON::Type::UNDEFINED:
               break;
            case BeeFishJSON::Type::_NULL:
               break;
            case BeeFishJSON::Type::BOOLEAN:
               path[DATA]
                  .getData(var._value._boolean);
               break;
            case BeeFishJSON::Type::NUMBER:
               path[DATA]
                  .getData(var._value._number);
               break;
            case BeeFishJSON::Type::STRING:
               path[DATA]
                  .getData(var._value._string);
               break;
            case BeeFishJSON::Type::ARRAY:
            {
               ArrayPointer array = ArrayPointer(new Array());
               var._value._array = array;
               Size max = this->max<Size>();

               for (Size i = 0; i <= max; ++i)
               {
                  JSONPath value = path[i];
                  array->push_back(
                     value.getVariable()
                  );
               }
               break;
            }
            case BeeFishJSON::Type::OBJECT:
            {
               BeeFishScript::ObjectPointer
               object = BeeFishScript::ObjectPointer(
                  new BeeFishScript::Object()
               );
               var._value._object = object;

               Path table = path[OBJECT_TABLE];
               Path map = path[OBJECT_VALUES];


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
         BeeFishJSON::Type type;
         getData(type);

         if (type == BeeFishJSON::Type::OBJECT)
         {
             path << OBJECT_VALUES;
         }

         path << key;
         
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
         BeeFishJSON::Type type;
         getData(type);


         if (type == BeeFishJSON::Type::OBJECT)
         {
             Path table = path[OBJECT_TABLE];
             String key;
             table.getData(key);
             path << OBJECT_VALUES << key;

         }
         else
            path << index;
         
         return JSONPath(path);
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