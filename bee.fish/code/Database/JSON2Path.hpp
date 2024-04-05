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
      
      virtual void setVariable(const Type type, const std::string& value = "")
      {

         cerr << "setVariable " << type << " " << value << endl;
         
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
               path.setData(Size(0));
               break;
            case Type::OBJECT:
               push_back_path(path);
               push_back_container(false);
               path.setData(Size(0));

               break;
            default:
               throw std::logic_error("JSON2Path");
         }
         
         pop_back_path();
         
             
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
    
      JSON2Path(Path start) :
         JSONParser(),
         _start(start)
      {
         push_back_path(_start);
      }
      
      JSON2Path(Database& database) :
         JSON2Path(Path(database))
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
      
      
      virtual void onbeginobject(JSON* match) 
      override {
         setVariable(Type::OBJECT);
         Path path =
            topPath()[Type::OBJECT];
            
         path.setData(Size(0));
         
         push_back_path(
            path
         );
      }
      
      virtual void onobjectkey(BString& key)
      override
      {
      
      }
      
      virtual void onendobject(JSON* match)
      override
      {
         pop_back_path();
      }

      virtual void onbeginarray(JSON* match)
      override
      {
         cerr << "onbeginarray" << endl;
         
         Path path =
            topPath()[Type::ARRAY];
            
         path.setData(Size(0));
         
         push_back_path(
            path
         );
      }

      virtual void onarrayvalue(JSON* json) {

         cerr << "onarrayvalue " ;

         Type type = json->type();
         
         Path path = topPath();
         
         cerr << path._index << " ";
         
         Size& count = path.getData();
         
         cerr << "[" << count << "] = " << json->type() << " " << json->value() << endl;
         
         path = path[count++][type];
         
         if (type == Type::ARRAY || type == Type::OBJECT)
         {
            path.setData(Size(0));
         }
         else
            path.setData(json->value());
         
         /*
         push_back_path(path);
         
         setVariable(
            json->type(),
            json->value()
         );
         */
            
         
      }

      virtual void onendarray(JSON* match) {
         cerr << "onendarray "  << endl;
         
         pop_back_path();
         pop_back_container();
        
      }

      virtual void onobjectvalue(BString& key, const JSON* json) {

         cerr << "onobjectvalue " << key << " " << json->type() << " " << json->value() << endl;
         
         Path path = topPath();
         Size& count = path.getData();
         count++;
         path = path[key];
         
         push_back_path(path);
         
         setVariable(
               json->type(),
               json->value()
            );
         
      }

      virtual void onvalue(JSON* json)
      override
      {
         cerr << "onvalue " << json->type() << " " << json->value() << endl;
         
         Type type = json->type();
         const BString& value = json->value();
         
         
         //if (_pathStack.size() == 1 && type != Type::ARRAY)
            setVariable(
               type,
               value.str()
            );
         
      }
               
      
      
   };

   

}

#endif
