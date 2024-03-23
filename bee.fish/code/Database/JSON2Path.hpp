#ifndef BEE_FISH_DATABASE_JSON_2_PATH_HPP
#define BEE_FISH_DATABASE_JSON_2_PATH_HPP
#include "../Database/DatabaseBase.hpp"
#include "../json/json.h"
#include "../json/json-parser.h"

namespace BeeFishDatabase {
    
   using namespace BeeFishParser;
   using namespace BeeFishScript;
   using namespace BeeFishJSON;
   
   class JSON2Path : public JSONParser
   {
   protected:
      
      Path _start;
      vector<Path> _pathStack;
      vector<bool> _containerStack;

      virtual void setVariable(const Type type, const std::string& value = "")
      {
         bool isArrayContainer = topContainer();
         
         if (isArrayContainer)
            setArrayVariable(type, value);
         else
            setObjectVariable(type, value);
            
         
      }
      
      virtual void setObjectVariable(Type type, const std::string& value = "")
      {

         Path path =
            topPath()[type];

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
            case Type::NUMBER:
            {
               path.setData<std::string>(
                  value
               );
               
               break;
            }
            case Type::STRING:
               path.setData(value);
               break;
            case Type::ARRAY:
               push_back_path(path);
               push_back_container(true);
               path.setData<Size>(0);
               break;
            case Type::OBJECT:
               push_back_path(path);
               push_back_container(false);
               path.setData<Size>(0);
               break;
            default:
               throw std::logic_error("JSON2Path");
         }
         
         
             
      }
      
      virtual void setArrayVariable(Type type, const std::string& value = "")
      {

         MinMaxPath path =
            topPath();
            
         Size& next = path.getData();

         path = path[next++];
         
         push_back_path(path);
         push_back_container(false);
         
         setVariable(type, value);
         
         
      }
          
   public:
    
      JSON2Path(Path start, Match& match)
         : JSONParser(match),
         _start(start)
      {
         push_back_path(_start);
         push_back_container(false);
      }
      
      JSON2Path(Database& database, Match& match) :
         JSON2Path(Path(database), match)
      {
      }

      virtual ~JSON2Path()
      {
      }
      
      Path topPath() {
         return 
            _pathStack[_pathStack.size() - 1];
      }
      
      bool topContainer() {
         return
            _containerStack
            [_containerStack.size() - 1];
      }
      
      virtual void push_back_path(Path path)
      {
         _pathStack.push_back(path);
      }
      
      virtual void push_back_container(bool isArrayContainer)
      {
         _containerStack.push_back(isArrayContainer);
      }
      
      virtual void pop_back_path()
      {
         _pathStack.pop_back();
      }
      
      virtual void pop_back_container()
      {
         _containerStack.pop_back();
      }
      
      virtual void onbeginobject(Match* match) {
         setVariable(Type::OBJECT);
         
      }
      
      virtual void onobjectkey(const BString& key)
      override
      {

         Path path =
            topPath()[key];
            
         push_back_path(path);
         push_back_container(false);
      
      }
      
      virtual void onendobject(Match* match) {
         pop_back_path();
         pop_back_container();
         Path path = topPath();
         Size count = 0;
         if (path.hasData())
            count = path.getData();
         ++count;
         path.setData(count);
      }

      virtual void onbeginarray(Match* match) {
         setVariable(Type::ARRAY);
      }

      virtual void onarrayvalue(Match* match) {

         pop_back_path();
         pop_back_container();
      }

      virtual void onendarray(Match* match) {
         pop_back_path();
         pop_back_container();
      }

      virtual void onobjectvalue(const BString& key, const JSON* value) {
         pop_back_path();
         pop_back_container();
         Path path = topPath();
         Size& count = path.getData();
         ++count;
        
      }

      virtual void onvalue(JSON* json) {
         if (json->type() == Type::ARRAY ||
             json->type() == Type::OBJECT)
            return;

         setVariable(
            json->type(),
            json->value()
         );
         
      }
               
      
      
   };

   

}

#endif
