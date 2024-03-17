#ifndef BEE_FISH__JSON_2_PATH_HPP
#define BEE_FISH__JSON_2_PATH_HPP
#include "../Database/Database.hpp"
#include "JSON.hpp"

namespace BeeFishJSON {
   using namespace BeeFishParser;
   using namespace BeeFishScript;

   class JSON2Path : public JSON
   {
   protected:
      
      Path _start;
      vector<Path>* _pathStack;
      vector<bool>* _containerStack;
      bool _deleteStack = false;
      
      virtual void setVariable(const Variable& var, const std::string& value = "")
      {
         bool isArrayContainer = topContainer();
         
         if (isArrayContainer)
            setArrayVariable(var, value);
         else
            setObjectVariable(var, value);
            
         
      }
      
      virtual void setObjectVariable(const Variable& var, const std::string& value = "")
      {
         Path path =
            topPath()[var._type];

         switch (var._type)
         {
            case Type::UNDEFINED:
               path["undefined"];
               break;
            case Type::NULL_:
               path["null"];
               break;
            case Type::BOOLEAN:
               path
                  [var._value._boolean
                     ? "true" : "false"];
               break;
            case Type::INTEGER:
               path
                  [var._value._integer];
               break;
            case Type::NUMBER:
            {
               path.setData(
                  value
               );
               
               break;
            }
            case Type::STRING:
               path.setData(var._value._string);
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
      
      virtual void setArrayVariable(const Variable& var, const std::string& value = "")
      {
         MinMaxPath path =
            topPath();
            
         Size next;
             
         if (path.isDeadEnd())
            next = 0;
         else
            next = path.max<Size>()
               + 1;

         path = path[next];
         
         push_back_path(path);
         push_back_container(false);
         
         setVariable(var, value);
         
         
      }
          
   public:
    
      JSON2Path(Path start)
         : JSON(),
         _start(start)
      {
         _pathStack = new vector<Path>();
         _containerStack = new vector<bool>();
         _deleteStack = true;
         push_back_path(_start);
         push_back_container(false);
      }
      
      JSON2Path(Database& database) :
          JSON2Path(Path(database))
      {
      }

      JSON2Path(const JSON2Path& source) :
         JSON(source),
         _start(source._start),
         _pathStack(source._pathStack),
         _containerStack(source._containerStack)
      {
      }

      virtual ~JSON2Path()
      {
         if (_deleteStack) {
            pop_back_path();
            delete _pathStack;
            delete _containerStack;
         }
      }
      
      static Parser* _JSON2Path(void* params) {
         return new JSON2Path(*(JSON2Path*)params);
      }
   
      Parser* copy() const override {
         return new JSON2Path(*this);
      }

      virtual LoadOnDemand createJSON(void* params) {
         return LoadOnDemand(_JSON2Path, this);
      }
      
      Path topPath() {
         return 
            (*_pathStack)[_pathStack->size() - 1];
      }
      
      bool topContainer() {
         return
            (*_containerStack)
            [_containerStack->size() - 1];
      }
      
      virtual void push_back_path(Path path)
      {
         _pathStack->push_back(path);
      }
      
      virtual void push_back_container(bool isArrayContainer)
      {
         _containerStack->push_back(isArrayContainer);
      }
      
      virtual void pop_back_path()
      {
         _pathStack->pop_back();
      }
      
      virtual void pop_back_container()
      {
         _containerStack->pop_back();
      }
      
      virtual bool onundefined(Parser* parser)
      {
         setVariable(undefined);
         return true;
      }
      
      virtual bool onnull(Parser* parser)
      {
         setVariable(null);
         return true;       
      }
      
      virtual bool onboolean(bool value, Parser* parser)
      {
         setVariable(
            Variable(value)
         );
         return true;
      }
                  
      virtual bool onnumber(BeeFishScript::Number& value, Parser* parser)
      {
         setVariable(
            Variable(Type::NUMBER),
            parser->value()
         );
         
         return true;
      }
      
      virtual bool oninteger(BeeFishScript::Integer& value, Parser* parser)
      {      
         setVariable(
            Variable(value)
         );
         return true;
      }
      
      virtual bool onstring(const BeeFishScript::String& value, Parser* parser)
      {      
         setVariable(
            Variable(value)
         );
         
         return true;
      }
      
      virtual bool onbeginarray(Parser* parser)
      {
         setVariable(Variable(Type::ARRAY));
         return true;
      }
      
      virtual bool onendarray(Parser* parser)
      {
         pop_back_path();
         pop_back_container();
         return true;
      }
      
      virtual bool onarrayitem(Parser* parser)
      {
          
         pop_back_path();
         pop_back_container();
         return true;
        
      }
      
      virtual bool onbeginobject(Parser* parser)
      {
         setVariable(Variable(Type::OBJECT));
         
         return true;
      }
      
      virtual bool onobjectkey(Parser* parser)
      {
         const BeeFishScript::String& key = parser->value();
         
         Path path =
            topPath()[key];
            
         push_back_path(path);
         push_back_container(false);
         
         return true;
      }
               
      virtual bool onobjectvalue(Parser* parser)
      {
         pop_back_path();
         pop_back_container();
         Path path = topPath();
         Size count = 0;
         if (path.hasData())
            count = path.getData();
         ++count;
         path.setData(count);
        
         return true;
         
      }
               
      virtual bool onendobject(Parser* parser)
      {
         pop_back_path();
         pop_back_container();
         return true;
      }
      
      
   };

   

}

#endif
