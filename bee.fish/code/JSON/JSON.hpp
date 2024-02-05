#ifndef BEE_FISH__JSON_H
#define BEE_FISH__JSON_H

#include "../Parser/Parser.hpp"
#include "../PowerEncoding/PowerEncoding.hpp"
#include "../Script/Variable.hpp"
#include "Version.hpp"
#include "Object.hpp"
#include "Array.hpp"
#include "Number.hpp"
#include "String.hpp"
#include "BlankSpace.hpp"

namespace BeeFishJSON {

   using namespace BeeFishParser;
   using namespace BeeFishScript;

   class JSON : public Parser
   {
   protected:
      Parser* _params {nullptr};
      And _parser;
      String _value;
      
      vector<Variable*> _stack;
      vector<String> _keyStack;
      
   public:
      using Parser::read;
      
      Variable* _variable {nullptr};
      
      JSON(Parser* params = nullptr) :
         _params(params),
         _parser(createParser(_params))
      {

      }

      JSON(const JSON& source) :
          _params(source._params),
          _parser(createParser(_params))
      {
      }
      
      virtual ~JSON()
      {
          if (_variable) {
             delete _variable;
             _variable = nullptr;
          }
      }

      Parser* copy() const override {
         return new JSON(*this);
      }

      virtual bool read(char c)
      override
      {

         if (_parser._result != nullopt)
            return false;

         Parser::read(c);

         bool matched = _parser.read(c);

         if (_parser._result != nullopt)
            setResult(_parser._result);

         return matched;
      }
      
      
      And createParser(Parser* params) {
        
         auto _undefined =
            Invoke(
               Word("undefined"),
               [this](Parser* parser) {
                  this->_variable =
                     new Variable();
                  return true;
               }
            );

         auto _null =
            Invoke(
               Word("null"),
               [this](Parser* parser)
               {
                  this->_variable =
                     new Variable(nullptr);
                     
                  return true;
               }
            );

         auto _true =
            Invoke(
               Word("true"),
               [this](Parser* parser)
               {
                  this->_variable =
                     new Variable(true);
                     
                  return true;
               }
            );
            
         auto _false =
            Invoke(
               Word("false"),
               [this](Parser* parser)
               {
                  this->_variable =
                     new Variable(false);
                     
                  return true;
               }
            );

         auto boolean =
            _true or _false;
            
         auto _number =
            Invoke(
                Capture(
                   number,
                   this->_value
                ),
                [this](Parser* parser) {
                   stringstream stream;
                   stream << _value;
                   Number number;
                   stream >> number;
                   this->_variable =
                      new Variable(number);
                   _value = "";
                   return true;
                }
            );
            
         auto __string =
            Invoke(
                Capture(
                   _string,
                   this->_value
                ),
                [this](Parser* parser) {
                   this->_variable =
                      new Variable(_value);
                   _value = "";
                   return true;
                }
            );
            
         auto _openBrace =
         Invoke(
            openBrace,
            [this](Parser*) {
               Variable* var =
                  new Variable(BeeFishScript::Object());
               _stack.push_back(var);
               return true;
            }
         );
         
         auto key =
         Invoke(
            Capture(
               _string,
               this->_value
            ),
            [this](Parser*) {
               _keyStack.push_back(_value);
               return true;
            }
         );
            
         auto objectValue =
         Invoke(
            LoadOnDemand(_JSON),
            [this](Parser* parser) {
                LoadOnDemand* loader = dynamic_cast<LoadOnDemand*>(parser);
                
                if (loader)
                   parser = loader->_loadOnDemand;
                   
                JSON* json = dynamic_cast<JSON*>(parser);
                
                if (json == nullptr ||
                    _keyStack.size() == 0)
                   return false;
                String key = _keyStack[_keyStack.size() - 1];
                _keyStack.pop_back();
                
                Variable* var =
                   _stack[_stack.size() - 1];
                   
                (*var)[key] = *(json->_variable);
                
                return true;
            }
         );
            
         auto keyValue =
            key and blankSpaces and
            Character(":") and
            blankSpaces and
            objectValue;
         
         auto _closeBrace =
         Invoke(
            closeBrace,
            [this](Parser*) {
                
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
         );
            
         auto object =
            blankSpaces and
            _openBrace and
            blankSpaces and
            Optional(
               keyValue and
               Repeat(
                  seperator and keyValue,
                  0
               )
            ) and
            blankSpaces and
            _closeBrace;
            
         return
            blankSpaces and
            (
               _undefined or
               _null or
               boolean or
               _number or
               __string or
               array or
               object
            );
      }

   };
   

   Parser* _JSON(Parser* params) {
      return JSON(params).copy();
   }

}


#endif