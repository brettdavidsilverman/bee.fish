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
      
      
      class StackPath : public Path
      {
      public:
          StackPath(const Path& path)
          : Path(path)
          {
          }
          
          virtual void setVariable(const Variable& var)
          {
              (*this)[var._type].setData(var);
          }
      };
      
      class StackArrayPath : public StackPath
      {
      public:
          StackArrayPath(const Path& path)
          : StackPath(path)
          {
          }
          
          virtual void setVariable(const Variable& var)
          {
             Path path = *this;
             MinMaxPath maxPath = path;
             Size next;
             if (maxPath.isDeadEnd())
                next = 0;
             else
                next = maxPath.max<Size>() + 1;
         
             StackPath stackPath =
                Path((*this)[next]);
             stackPath.setVariable(var);
          }
      };
      
      vector<StackPath*> _stack;
     
   public:
    
      JSON2Path(Path start, void* params = nullptr)
         : JSON(params),
         _start(start)
      {
         _stack.push_back(new StackPath(_start));
      }

      virtual ~JSON2Path()
      {
          for (auto path : _stack)
             delete path;
      }

      JSON2Path(const JSON2Path& source) :
         JSON(source)
      {
          _stack.push_back(new StackPath(_start));
      }

      static Parser* _JSONPath(void* params) {
         return new JSON2Path(*(Path*)params, params);
      }
   
      Parser* copy() const override {
         return new JSON2Path(*this);
      }

      virtual LoadOnDemand createJSON(void* params) {
         return LoadOnDemand(_JSONPath, &_start);
      }
      
      StackPath* top() {
         assert(_stack.size() > 0);
         return
            _stack[_stack.size() - 1];
      }
      
      virtual bool onundefined(Parser* parser)
      {
         top()->setVariable(undefined);
         return true;
      }
      
      virtual bool onnull(Parser* parser)
      {
         top()->setVariable(
            Variable(nullptr)
         ); 
         return true;       
      }
      
      virtual bool onboolean(bool value, Parser* parser)
      {
         top()->setVariable(
            Variable(value)
         );
         return true;
      }
                  
      virtual bool onnumber(BeeFishScript::Number& value, Parser* parser)
      {      
         top()->setVariable(
            Variable(value)
         );
         
         return true;
      }
      
      virtual bool oninteger(BeeFishScript::Integer& value, Parser* parser)
      {      
         top()->setVariable(
            Variable(value)
         );
         return true;
      }
      
      virtual bool onstring(const BeeFishScript::String& value, Parser* parser)
      {      
         top()->setVariable(
            Variable(value)
         );
         return true;
      }
      
      virtual bool onbeginobject(Parser* parser)
      {      
      
         StackPath* object = new StackPath(
            (*top())[Type::OBJECT]
         );
         _stack.push_back(object);
         
         return true;
      }
      
      virtual bool onobjectkey(Parser* parser)
      {
         BeeFishScript::String key = parser->value();
         StackPath* path = new StackPath(
            (*top())[key]
         );
         _stack.push_back(path);
         return true;
      }
               
      virtual bool onobjectvalue(Parser* parser)
      {
         Path* path = top();
         _stack.pop_back();
         delete path;
         return true;
         
      }
               
      virtual bool onendobject(Parser* parser)
      {
         if (_stack.size() == 0)
            return false;
                  
         Path* path = top();
         
         _stack.pop_back();
         
         delete path;
         
         return true;
      }
      
      virtual bool onbeginarray(Parser* parser)
      {
         StackPath* array = new StackArrayPath(
            (*top())[Type::ARRAY]
         );
         _stack.push_back(array);
         return true;
      }
      
      virtual bool onendarray(Parser* parser)
      {
         if (_stack.size() == 0)
            return false;
         Path* path = top();
         _stack.pop_back();
         delete path;
         return true;
      }
      
      virtual bool onarrayitem(Parser* parser)
      {
          
         if (_stack.size() == 0)
            return false;
         
         return true;
        
      }
      
   };

   

}

#endif
