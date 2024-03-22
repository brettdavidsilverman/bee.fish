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
      void* _params {nullptr};
      std::string _numberString;
      std::string _integerString;
      Parser* _parser;
      
      
   public:
      using Parser::read;
       
      JSON(void* params = nullptr) :
         _params(params),
         _parser(nullptr)
      {

      }

      JSON(const JSON& source) :
          _params(source._params),
          _parser(nullptr)
      {
      }
      
      virtual ~JSON()
      {
         if (_parser) {
            delete _parser;
            _parser = nullptr;
         }
      }

      Parser* copy() const override {
         return new JSON(*this);
      }

      virtual bool read(char c)
      override
      {
         if (_parser == nullptr)
            _parser = createParser(_params);
            
         return readIndirect(*_parser, c);
      }
      
      virtual LoadOnDemand createJSON(void* params) {
         return LoadOnDemand(_JSON, params);
      }
      
      virtual bool onundefined(Parser* parser)
      {
         return true;
      }
      
      virtual bool onnull(Parser* parser)
      {
         return true;       
      }
      
      virtual bool onboolean(bool value, Parser* parser)
      {
         return true;
      }
                  
      virtual bool onnumber(BeeFishScript::Number& value, Parser* parser)
      {
         return true;
      }
      /*
      virtual bool oninteger(BeeFishScript::Integer& value, Parser* parser)
      {      
         return true;
      }
      */
      virtual bool onstring(const BeeFishScript::String& value, Parser* parser)
      {      
         return true;
      }
      
      virtual bool onbeginobject(Parser* parser)
      {      
         return true;
      }
      
      virtual bool onobjectkey(Parser* parser)
      {
         return true;
      }
               
      virtual bool onobjectvalue(Parser* parser)
      {
         return true;
      }
               
      virtual bool onendobject(Parser* parser)
      {
         return true;
      }
      
      virtual bool onbeginarray(Parser* parser)
      {
         return true;
      }
      
      virtual bool onendarray(Parser* parser)
      {
         return true;
      }
      
      virtual bool onarrayitem(Parser* parser)
      {
         return true;
      }
               
      virtual Parser* createParser(void* params) {
        
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
                   
                   Number number;
                      stream >> number;
                   onnumber(number, parser);
                      
                   /*
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
                   */
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
            createArrayParser(params);
            
         const auto object =
            createObjectParser(params);
            
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
      
      And createObjectParser(void* params) {
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
            createJSON(params),
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
      
      
      And createArrayParser(void* params) {
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
               createJSON(params),
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
   
   Parser* _JSON(void* params) {
      return new JSON(params);
   }
   

}


#endif