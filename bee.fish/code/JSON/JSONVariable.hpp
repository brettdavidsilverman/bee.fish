#ifndef BEE_FISH__JSON_VARIABLE_HPP
#define BEE_FISH__JSON_VARIABLE_HPP

#include "JSON.hpp"
#include "JSONVariable.hpp"

namespace BeeFishJSON {
   using namespace BeeFishParser;
   using namespace BeeFishScript;

   class JSONVariable : public JSON
   {
   protected:
      vector<Variable*> _stack;
      vector<BeeFishScript::String> _keyStack;
   public:
      Variable* _variable {nullptr};
    
      JSONVariable(void* params = nullptr)
         : JSON(params)
      {
      }

      virtual ~JSONVariable()
      {
         if (_variable) {
            delete _variable;
            _variable = nullptr;
         }
      }

      JSONVariable(const JSONVariable& source) :
         JSON(source)
      {
      }

      static Parser* _JSONVariable(void* params) {
         return new JSONVariable(params);
      }
   
      Parser* copy() const override {
         return new JSONVariable(*this);
      }

      virtual LoadOnDemand createJSON(void* params) {
         return LoadOnDemand(_JSONVariable, params);
      }
      
      virtual bool onundefined(Parser* parser)
      {
         this->_variable =
            new Variable();
         return true;
      }
      
      virtual bool onnull(Parser* parser)
      {
         this->_variable =
            new Variable(nullptr);
                     
         return true;       
      }
      
      virtual bool onboolean(bool value, Parser* parser)
      {
         this->_variable =
            new Variable(value);
                     
         return true;
      }
                  
      virtual bool onnumber(BeeFishScript::Number& value, Parser* parser)
      {      
         this->_variable =
            new Variable(value);
            
         return true;
      }
      
      virtual bool oninteger(BeeFishScript::Integer& value, Parser* parser)
      {      
         this->_variable =
            new Variable(value);
            
         return true;
      }
      
      virtual bool onstring(const BeeFishScript::String& value, Parser* parser)
      {      
         this->_variable =
            new Variable(value);
            
         return true;
      }
      
      virtual bool onbeginobject(Parser* parser)
      {      
      
         Variable* var =
            new Variable(BeeFishScript::Object());
         _stack.push_back(var);
         
         return true;
      }
      
      virtual bool onobjectkey(Parser* parser)
      {
         BeeFishScript::String key = parser->value();
         _keyStack.push_back(key);
         return true;
      }
               
      virtual bool onobjectvalue(Parser* parser)
      {
         LoadOnDemand* loader =
            dynamic_cast<LoadOnDemand*>(parser);
                
         if (loader)
            parser = loader->_loadOnDemand;
                   
         JSONVariable* json =
            dynamic_cast<JSONVariable*>(parser);
                
         if (json == nullptr ||
             _keyStack.size() == 0)
            return false;
                   
         BeeFishScript::String key =
            _keyStack[
               _keyStack.size() - 1
            ];
                   
         _keyStack.pop_back();

         Variable* var =
            _stack[_stack.size() - 1];
                   
         (*var)[key] = *(json->_variable);
                
         return true;
                
      }
               
      virtual bool onendobject(Parser* parser)
      {
         if (_stack.size() == 0)
            return false;
                  
         Variable* var =
            _stack[_stack.size() - 1];
         _stack.pop_back();
         
         if (_stack.size() == 0)
            _variable = var;
         else
            delete var;
         
         return true;
      }
      
      virtual bool onbeginarray(Parser* parser)
      {
         Variable* var =
            new Variable(
               BeeFishScript::Type::ARRAY
            );
         _stack.push_back(var);
         return true;
      }
      
      virtual bool onendarray(Parser* parser)
      {
         if (_stack.size() == 0)
            return false;
                  
         Variable* var =
            _stack[_stack.size() - 1];
         _stack.pop_back();
         if (_stack.size() == 0)
            _variable = var;
         else
            delete var;
         return true;
      }
      
      virtual bool onarrayitem(Parser* parser)
      {
         if (_stack.size() == 0)
            return false;
                  
         Variable* var =
            _stack[_stack.size() - 1];
                     
         ArrayPointer array = (*var);
                  
         LoadOnDemand* load =
            dynamic_cast
            <LoadOnDemand*>(parser);
                  
         JSONVariable* json = nullptr;
                  
         if (load && load->_loadOnDemand)
         {
            json =
               dynamic_cast
               <JSONVariable*>(load->_loadOnDemand);
         }
                  
         if (json && json->_variable)
         {
            array->push_back(*(json->_variable));
            delete json->_variable;
            json->_variable = nullptr;
         }
         else
         {
            array->push_back(undefined);
         }
         return true;
      }
   };

   

}

#endif
