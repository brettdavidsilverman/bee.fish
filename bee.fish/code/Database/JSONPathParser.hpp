#ifndef BEE_FISH_DATABASE_JSON_PARSER_HPP
#define BEE_FISH_DATABASE_JSON_PARSER_HPP
#include "../Database/DatabaseBase.hpp"
#include "../json/json.h"
#include "../json/json-parser.h"
#include "JSONPath.hpp"

namespace BeeFishDatabase {
 
   using namespace BeeFishParser;
   using namespace BeeFishScript;
   using namespace BeeFishJSON;

   /*
   db[OBJECTS][objectPath][position][type][value]
   db[PROPERTIES][key][VALUE][type][value][objectPath]
   db[PROPERTIES][key][POSITION][objectPath] = position;
   */
   
   
   class JSONPathParser :
      public JSONParser,
      public JSONPath
   {
   protected:
      
      vector<Path> _pathStack;
      vector<bool> _containerStack;
      vector<Index> _arrayIndexStack;
      vector<BString> _keyStack;
      vector<Index> _objectPropertyIndexStack;

   public:
       

      JSONPathParser(Path& path) :
         JSONParser(),
         JSONPath(path)
      {
        // push_back_path(path);
       //  push_back_container(false);
      }

      virtual ~JSONPathParser()
      {
         //pop_back_path();
        // pop_back_container();
      }
      
      virtual optional<bool> read(const char* str)
      {
         return JSONParser::read(str);
      }
          
      /*
   _objects[objectId][position][type][value]
   
   */
      
   protected:
      
      virtual void setVariable(Path path, const Type type, const BString& value = "")
      {
         /*
         Path valuePath;
         
         bool isArrayContainer = topContainer();
         if (isArrayContainer)
         {
             // Get next array index
             Index& index = topArrayIndex();
             valuePath = topPath()[index++];
             
         }
         else if (_keyStack.size())
         {
            BString& key = topKey();
            JSONPath jsonPath(topPath());
            valuePath = jsonPath[key];
            pop_back_key();
         }
         else
            valuePath = topPath();
            
         */
         Path valuePath = path[type];
       
         switch (type)
         {
            case Type::UNDEFINED:
               valuePath.deleteData();
               break;
            case Type::NULL_:
            case Type::BOOLEAN:
            case Type::INTEGER:
            case Type::NUMBER:
            case Type::STRING:
               valuePath.setData(value);
               break;
            case Type::OBJECT:
            case Type::ARRAY:
               break;
            default:
               throw std::logic_error("JSONPath");
         }

             
     }

   public:
    
      
      Path& topPath() {
          
         if (_pathStack.size() == 0)
            return *this;
            
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

      Index& topObjectPropertyIndex() {
         return
            _objectPropertyIndexStack
            [_objectPropertyIndexStack.size() - 1];
      }

      Index& topArrayIndex() {
         return 
            _arrayIndexStack
            [_arrayIndexStack.size() - 1];
      }
      
      void push_back_path(const Path& path)
      {
         _pathStack.push_back(path);
      }
      
      void push_back_container(bool isArrayContainer)
      {
         _containerStack.push_back(isArrayContainer);
         //if (isArrayContainer)
            
         //else
         //   _objectPropertyIndexStack.push_back(0);
      }
      
      void push_back_array_index(Index start = 0)
      {
         _arrayIndexStack.push_back(start);
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
         _containerStack.pop_back();
      }
      
      void pop_back_array_index()
      {
         _arrayIndexStack.pop_back();
      }

      void pop_back_key()
      {
         _keyStack.pop_back();
      }
      
      
      virtual void onbeginobject(JSON* match) 
      override {
 
         push_back_path(topPath()[Type::OBJECT]);
         push_back_container(false);
         
         
      }
      
      virtual void onobjectkey(BeeFishJSON::Object* object, ObjectKey* key)
      override
      {
         push_back_key(key->value());
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

         push_back_path(topPath()[Type::ARRAY]);
         push_back_container(true);
         push_back_array_index(0);
      }

      virtual void onarrayvalue(JSON* json) 
      {
         // Get next array index
         Index& index = topArrayIndex();
         cerr << index << ": " << json->type() << ": " << json->value() << endl;
         Path path = topPath()[index++];
         setVariable(path, json->type(), json->value());
      }

      virtual void onendarray(JSON* match) 
      {
          
         pop_back_path();
         pop_back_container();
         pop_back_array_index();
      }

      virtual void onobjectvalue(BString& key, const JSON* json)
      {
         JSONPath jsonPath(topPath());
         Path valuePath = jsonPath[key];
         setVariable(valuePath, json->type(), json->value());
      }
      
      virtual void onvalue(JSON* json)
      override
      {
         
         if ( _pathStack.size() == 0 )
         {
            setVariable(
               topPath(),
               json->type(),
               json->value()
            );
         }
         
 
      }
               
      
      
   };

   

}

#endif
