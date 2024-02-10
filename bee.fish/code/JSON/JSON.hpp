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
      
      vector<Variable*> _stack;
      vector<std::string> _keyStack;
      
   public:
      using Parser::read;
      
      Variable* _variable {nullptr};
      
      JSON(Parser* params = nullptr) :
         _params(params),
         _parser(createVariableParser(_params))
      {

      }

      JSON(const JSON& source) :
          _params(source._params),
          _parser(createVariableParser(_params))
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

         return readIndirect(_parser, c);
      }
      
      And createVariableParser(Parser* params) {
        
         const auto _undefined =
            Invoke(
               Word("undefined"),
               [this](Parser* parser) {
                  this->_variable =
                     new Variable();
                  return true;
               }
            );

         const auto _null =
            Invoke(
               Word("null"),
               [this](Parser* parser)
               {
                  this->_variable =
                     new Variable(nullptr);
                     
                  return true;
               }
            );

         const auto _true =
            Invoke(
               Word("true"),
               [this](Parser* parser)
               {
                  this->_variable =
                     new Variable(true);
                     
                  return true;
               }
            );
            
         const auto _false =
            Invoke(
               Word("false"),
               [this](Parser* parser)
               {
                  this->_variable =
                     new Variable(false);
                     
                  return true;
               }
            );

         const auto _boolean =
            _true or _false;
            
         const auto _number =
            Invoke(
                Capture(
                   number,
                   this->_value),
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


         const auto __string =
            Invoke(
               _string,
               [this](Parser* parser) {
                  this->_variable =
                     new Variable(
                        parser->_value
                     );
                   return true;
                }
            );
            
         const auto array =
            createArrayParser();
            
            
         const auto object =
            createObjectParser();
            
         return
            blankSpaces and
            (
               _undefined or
               _null or
               _boolean or
               _number or
               __string or
               array or
               object
            );
      }
      
      And createObjectParser() {
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
            _string,
            [this](Parser* parser) {
               _keyStack.push_back(
                  parser->value()
               );
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
                   
                std::string key =
                   _keyStack[
                      _keyStack.size() - 1
                   ];
                   
                _keyStack.pop_back();
                
                //key = unescape(key);
                
                Variable* var =
                   _stack[_stack.size() - 1];
                   
                (*var)[key] = *(json->_variable);
                
                return true;
            }
         );
            
         auto keyValue =
         key and blankSpaces and
         colon and
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
               blankSpaces and
               seperator and
               blankSpaces and
               keyValue,
               0
            )
         ) and
         blankSpaces and
         _closeBrace;
         
         return object;
      }
      
      And createArrayParser() {
         const auto _openBracket =
            Invoke(
               openBracket,
               [this](Parser*) {
                  Variable* var =
                     new Variable(
                        BeeFishScript::Type::ARRAY
                     );
                  _stack.push_back(var);
                  return true;
               }
            );
            

         const auto _closeBracket =
            Invoke(
               closeBracket,
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

         const auto _arrayItem =
            Invoke(
               arrayItem,
               [this](Parser* parser)
               {
                  if (_stack.size() == 0)
                     return false;
                  
                  Variable* var =
                     _stack[_stack.size() - 1];
                     
                  ArrayPointer array = (*var);
                  
                  JSON* value = nullptr;
                  Optional* optional = dynamic_cast<Optional*>(parser);
                  LoadOnDemand* load = dynamic_cast<LoadOnDemand*>(optional->_optional);
                  
                  if (load && load->_loadOnDemand)
                     value = dynamic_cast<JSON*>(load->_loadOnDemand);
                     
                  if (value && value->_variable)
                     array->push_back(*(value->_variable));
                  else
                     array->push_back(undefined);
                     
                  return true;
               }
            );
            

         const auto _arrayItems =
            _arrayItem and
            Repeat(
               arraySeperator and _arrayItem,
               0
            );

         const auto _array =
            _openBracket and
            Optional(
               _arrayItems
            ) and
            _closeBracket;
            
         return _array;
      
      }

   };
   

   Parser* _JSON(Parser* params) {
      return JSON(params).copy();
   }
   
   

}


#endif