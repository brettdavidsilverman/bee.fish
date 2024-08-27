#ifndef BEE_FISH_DATABASE_JSON_2_PATH_HPP
#define BEE_FISH_DATABASE_JSON_2_PATH_HPP
#include "../Database/DatabaseBase.hpp"
#include "../json/json.h"
#include "../json/json-parser.h"

namespace BeeFishDatabase {
 
   using namespace BeeFishParser;
   using namespace BeeFishScript;
   using namespace BeeFishJSON;
   
   class JSON2Path :
      public JSONParser
   {
   protected:
      
      Path _start;
      vector<Path> _pathStack;
      vector<bool> _containerStack;
      vector<Size> _arrayIndexStack;
      vector<std::string> _keyStack;
      
   public:

      JSON2Path(const Path& start) :
         JSONParser(),
         _start(start)
         //_properties(_start[0])
      {
         push_back_path(_start);
         push_back_container(false);
      }
      
      JSON2Path(Database& database) :
         JSON2Path(Path(database))
      {
      }

      virtual ~JSON2Path()
      {
         pop_back_path();
      }

   protected:
      virtual void setVariable(const Type type, const std::string& value = "")
      {

         Path path =
            topPath();
            
            
         bool isArrayContainer = topContainer();
         if (isArrayContainer)
         {
             // Get next array index
             Size index = topArrayIndex()++;
             path = path[index];
             
         }
         else if (_keyStack.size()) {
             //Size count = path.getData();
             //path.setData(++count);
             std::string key = topKey();
             pop_back_key();
             path = path[key];
         }
         
         setVariable(path, type, value);
     }
     
     virtual void setVariable(Path path, const Type type, const std::string& value)
     {
         
         path = path[type];

         switch (type)
         {
            case Type::UNDEFINED:
               break;
            case Type::NULL_:
               break;
            case Type::BOOLEAN:
               path.setData<bool>(
                  (value == "true") ? true : false
               );
               break;
            case Type::INTEGER:
               path.setData(
                  value
               );
               break;
            case Type::NUMBER:
               path.setData(
                  value
               );
               break;
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
               //path.setData(Size(0));
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
      
      std::string topKey() {
         return
            _keyStack
            [_keyStack.size() - 1];
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
         {
            _arrayIndexStack.push_back(0);
         }
      }

      void push_back_key(const std::string& key)
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
