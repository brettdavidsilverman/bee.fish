#ifndef BEE_FISH_DATABASE_JSON_2_PATH_HPP
#define BEE_FISH_DATABASE_JSON_2_PATH_HPP
#include "../Database/DatabaseBase.hpp"
#include "../json/json.h"
#include "../json/json-parser.h"
#include "JSONIndex.hpp"

namespace BeeFishDatabase {
 
   using namespace BeeFishParser;
   using namespace BeeFishScript;
   using namespace BeeFishJSON;

   class JSON2Path :
      public JSONParser
   {
   protected:
      
      Path _objects;
      Path _properties;
      vector<Path> _pathStack;
      vector<bool> _containerStack;
      vector<Size> _arrayIndexStack;
      vector<BString> _keyStack;
      vector<Size> _objectPropertyIndexStack;

   public:

      JSON2Path(const Path& objects, const Path& properties) :
         JSONParser(),
         _objects(objects),
         _properties(properties)
      {
         push_back_path(_objects);
         push_back_container(false);
      }
      
      JSON2Path(Database& database) :
         JSON2Path(
            Path(database)[OBJECTS],
            Path(database)[PROPERTIES]
         )
      {
      }

      virtual ~JSON2Path()
      {
         pop_back_path();
         pop_back_container();
         assert(_pathStack.size() == 0);
         assert(_containerStack.size() == 0);
         assert(_arrayIndexStack.size() == 0);
         assert(_keyStack.size() == 0);
         assert(_objectPropertyIndexStack.size() == 0);
      }

   protected:
      virtual void setVariable(const Type type, const BString& value = "")
      {

         Path path =
            topPath();
            
         bool isArrayContainer = topContainer();
         if (isArrayContainer)
         {
             // Get next array index
             Size& index = topArrayIndex();
             path = path[index++];
             
         }
         else if (_keyStack.size()) {
            BString& key = topKey();
            Size& count = topObjectPropertyIndex();
            Path property;

            if (_properties.contains(key))
               property = _properties[key];
            else {
               property = _properties[key];
               property.setData(key);
            }
            path = path[count++];
            path.setData(property.index());
            pop_back_key();
         }
         
         setVariable(path, type, value);

     }
     
     virtual void setVariable(Path path, const Type type, const BString& value)
     {
         
         path = path[type];

         switch (type)
         {
            case Type::UNDEFINED:
               break;
            case Type::NULL_:
            case Type::BOOLEAN:
            case Type::INTEGER:
            case Type::NUMBER:
            case Type::STRING:
               path.setData(value);
               break;
            case Type::ARRAY:
               push_back_path(path);
               push_back_container(true);
               break;
            case Type::OBJECT:
               push_back_path(path);
               push_back_container(false);
               break;
            default:
               throw std::logic_error("JSON2Path");
         }

             
      }

      
   public:
    
      
      Path topPath() {
         return 
            _pathStack[_pathStack.size() - 1];
      }
      
      bool topContainer() {
         return
            _containerStack
            [_containerStack.size() - 1];
      }
      
      BString& topKey() {
         return
            _keyStack
            [_keyStack.size() - 1];
      }

      Size& topObjectPropertyIndex() {
         return
            _objectPropertyIndexStack
            [_objectPropertyIndexStack.size() - 1];
      }

      Size& topArrayIndex() {
         return 
            _arrayIndexStack
            [_arrayIndexStack.size() - 1];
      }
      
      void push_back_path(Path path)
      {
         _pathStack.push_back(path);
      }
      
      void push_back_container(bool isArrayContainer)
      {
         _containerStack.push_back(isArrayContainer);
         if (isArrayContainer)
            _arrayIndexStack.push_back(0);
         else
            _objectPropertyIndexStack.push_back(0);
      }

      void push_back_key(const BString& key)
      {
         _keyStack.push_back(key);
      }
      
      void pop_back_path()
      {
         _pathStack.pop_back();
      }
      
      void pop_back_container()
      {
         bool isArrayContainer = topContainer();
         if (isArrayContainer) {
            _arrayIndexStack.pop_back();
         }
         else 
            _objectPropertyIndexStack.pop_back();

         _containerStack.pop_back();
      }

      void pop_back_key()
      {
         _keyStack.pop_back();
      }
      
      
      virtual void onbeginobject(JSON* match) 
      override {
 
         setVariable(
            Type::OBJECT
         );
         
      }
      
      virtual void onobjectkey(BString& key)
      override
      {
         push_back_key(key);
         key.clear();
      }
      
      virtual void onendobject(JSON* match)
      override
      {
         pop_back_path();
         pop_back_container();
      }

      virtual void onbeginarray(JSON* match)
      override
      {

         setVariable(
            Type::ARRAY
         );
         
      }

      virtual void onarrayvalue(JSON* json) {

      }

      virtual void onendarray(JSON* match) {
          
         pop_back_path();
         pop_back_container();
        
      }

      virtual void onobjectvalue(BString& key, const JSON* json) {

         
      }
      
      virtual void onvalue(JSON* json)
      override
      {
          
         if (json->type() != Type::ARRAY &&
             json->type() != Type::OBJECT)
         {
            setVariable(
               json->type(),
               json->value()
            );
         }
         
         
      }
               
      
      
   };

   

}

#endif
