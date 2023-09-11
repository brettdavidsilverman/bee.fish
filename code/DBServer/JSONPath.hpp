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
      const int OBJECT_TABLE = 0;
      const int OBJECT_VALUES = 1;

   public:
      JSONPath(const Path& source) :
         Path(source)
      {
      }

      JSONPath& operator = (const Variable& source)
      {
         setData(source);

         switch (source._type) {
            case BeeFishJSON::Type::UNDEFINED:
               break;
            case BeeFishJSON::Type::_NULL:
               break;
            case BeeFishJSON::Type::BOOLEAN:
               break;
            case BeeFishJSON::Type::NUMBER:
               break;
            case BeeFishJSON::Type::STRING:
               break;
            case BeeFishJSON::Type::ARRAY:
            {
               ArrayPointer array (source._value._array);
               for (Size i = 0; i < array->size(); ++i)
               {
                  (*this)[i] = (*array)[i];
               }
               break;
            }
            case BeeFishJSON::Type::OBJECT:
            {
               ObjectPointer object =
                  (source._value._object);
               JSONPath table = (*this)[OBJECT_TABLE];
               JSONPath values = (*this)[OBJECT_VALUES];
               Size i = 0;
               for (auto it = object->_table.cbegin(); it != object->_table.cend(); ++it)
               {
                  
                  String key = *it;
                  table[i++].setData(key);
                  values[key] = (*object)[key];
               }

               break;
            }
            default:
               throw std::logic_error("JSON::Variable::Value::copy constructor");
         }
         
         return *this;
      }

      JSONPath operator [] (const string& key)
      {
         Path path(*this);
         
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
         
         path << index;
         
         return JSONPath(path);
      }

      Variable variable() {
          Variable var;
          getData(var);
          switch (var._type) {
            case BeeFishJSON::Type::UNDEFINED:
               break;
            case BeeFishJSON::Type::_NULL:
               break;
            case BeeFishJSON::Type::BOOLEAN:
               break;
            case BeeFishJSON::Type::NUMBER:
               break;
            case BeeFishJSON::Type::STRING:
               break;
            case BeeFishJSON::Type::ARRAY:
            {
               ArrayPointer array (var._value._array);
               Size max = this->max<Size>();
               for (Size i = 0; i <= max; ++i)
               {
                  (*array)[i] = (*this)[i].variable();
               }
               break;
            }
            case BeeFishJSON::Type::OBJECT:
            {
               ObjectPointer object(var._value._object);

               JSONPath table = (*this)[OBJECT_TABLE];
               JSONPath values = (*this)[OBJECT_VALUES];


               Path::Stack stack;
               String key;
               Size i;
               while (this->next(stack, i))
               {
                  table[i].getData(key);
                  (*object)[key] = values[key].variable();
               }
               break;
            }
            default:
               throw std::logic_error("JSON::Variable::Value::copy constructor");
         }
         return var;
      }


   };

}

#endif