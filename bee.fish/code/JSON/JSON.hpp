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
      std::string _numberString;
      std::string _integerString;
      Parser* _parser;
      
      vector<Variable*> _stack;
      vector<BeeFishScript::String> _keyStack;
      
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
          delete _parser;
          
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

         return readIndirect(*_parser, c);
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
                   
         JSON* json = dynamic_cast<JSON*>(parser);
                
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
                  
         JSON* json = nullptr;
                  
         if (load && load->_loadOnDemand)
         {
            json =
               dynamic_cast
               <JSON*>(load->_loadOnDemand);
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
               
      Parser* createVariableParser(Parser* params) {
        
         const auto _undefined =
            Invoke(
               Word("undefined"),
               [this](Parser* parser)
               {
                  return onundefined(parser);
               }
            );

         const auto _null =
            Invoke(
               Word("null"),
               [this](Parser* parser)
               {
                  return onnull(parser);
               }
            );

         const auto _true =
            Invoke(
               Word("true"),
               [this](Parser* parser)
               {
                  return onboolean(true, parser);
               }
            );
            
         const auto _false =
            Invoke(
               Word("false"),
               [this](Parser* parser)
               {
                  return onboolean(false, parser);
               }
            );

         const auto _boolean =
            _true or _false;
                        
         const auto _number =
            Invoke(
                Capture(
                   number,
                   _numberString
                ),
                [this](Parser* parser) {
                   stringstream stream;
                   stream << _numberString;
                   if ( ( _numberString.find('.') !=
                           std::string::npos ) ||
                        ( _numberString.find('e') !=
                           std::string::npos ) ||
                        ( _numberString.find('E') !=
                          std::string::npos ) )
                   {
                      // Number contains decimal
                      // place or exponent.
                      // Treat it as float
                      Number number;
                      stream >> number;
                      onnumber(number, parser);
                   }
                   else
                   {
                      // Treat number as integral
                      Integer integer;
                      stream >> integer;
                      
                      oninteger(integer, parser);
                      
                   }
                   
                   _numberString = "";
                   
                   return true;
                }
            );


         const auto __string =
            Invoke(
               _string,
               [this](Parser* parser) {
                  return onstring(parser->value(), parser);
               }
            );
            
         const auto array =
            createArrayParser();
            
         const auto object =
            createObjectParser();
            
         return (
            -blankSpaces and
            (
               _undefined or
               _null or
               _boolean or
               _number or
               __string or
               array or
               object
            )
         ).copy();
      }
      
      And createObjectParser() {
         auto _openBrace =
         Invoke(
            openBrace,
            [this](Parser* parser) {
               return onbeginobject(parser);
            }
         );
         
         auto key =
         Invoke(
            _string,
            [this](Parser* parser) {
               return onobjectkey(parser);
            }
         );
            
         auto objectValue =
         Invoke(
            LoadOnDemand(_JSON),
            [this](Parser* parser) {
               return onobjectvalue(parser);
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
            [this](Parser* parser) {
               return onendobject(parser);
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
               [this](Parser* parser) {
                  return onbeginarray(parser);
               }
            );
            

         const auto _closeBracket =
            Invoke(
               closeBracket,
               [this](Parser* parser) {
                  return onendarray(parser);
               }
            );

         const auto _arrayItem =
            Invoke(
               LoadOnDemand(_JSON),
               [this](Parser* parser)
               {
                  return onarrayitem(parser);
               }
            );
            

         const auto _arrayItems =
            _arrayItem and
            Repeat(
               -blankSpaces and
               arraySeperator and
               -blankSpaces and
               _arrayItem,
               0
            );
            
         const auto _array =
            -blankSpaces and
            _openBracket and
               -blankSpaces and
               -_arrayItems and
               -blankSpaces and
            _closeBracket;
            
         return _array;
      
      }

   };
   

   Parser* _JSON(Parser* params) {
      return new JSON(params);
   }
   
   

}


#endif