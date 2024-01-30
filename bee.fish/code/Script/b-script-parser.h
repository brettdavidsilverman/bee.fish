#ifndef BEE_FISH__B_SCRIPT_PARSER_H
#define BEE_FISH__B_SCRIPT_PARSER_H

//#include "../database/path.h"
#include "../parser/parser.h"
#include "../json/json.h"
#include "../json/json-parser.h"

#include "object.h"

namespace BeeFishBScript
{
   
   using namespace BeeFishBString;

#ifdef SERVER
//   typedef BeeFishDatabase::Path<BeeFishPowerEncoding::PowerEncoding> Path;
#endif

   class BScriptParser : public BeeFishJSON::JSONParser
   {
   public:

      class Stack : public std::vector<Variable> {
         public:
            Stack() {

            }

            Variable pop() {
               Variable last = (*this)[size() - 1];
               pop_back();
               return last;
            }

            void push(const Variable& variable) {
               push_back(variable);
            }

            Variable& root() {
               
               if (size() > 0)
                  return (*this)[0];
                  
               throw std::runtime_error("No root variable");
            }

            Variable& top() {
               return (*this)[size() - 1];
            }

            size_t size() {
               return std::vector<Variable>::size();
            }

      };

   public:

      Stack _stack;


      BScriptParser(Match& match) :
         JSONParser(match)
      {
         _result = BeeFishMisc::nullopt;
      }
      
      virtual ~BScriptParser()
      {
      }
      
      virtual bool isBScriptParser() {
         return true;
      }

      virtual bool matched() {
         return _match.matched();
      }

      Variable& value() {
         return _stack.root();
      }

      Variable& json() {
         return _stack.root();
      }


      virtual void onvalue(BeeFishJSON::JSON* json) {

         switch (json->type()) {
            case BeeFishJSON::OBJECT:
               if (_stack.size() == 0)
                  _stack.push(ObjectPointer(new Object));
               break;
            case BeeFishJSON::ARRAY:
               if (_stack.size() == 0)
                  _stack.push(ArrayPointer(new Array));
               break;
            default:
               Variable value = createVariable(json);
               _stack.push(value);
         }

         BeeFishJSON::JSONParser::onvalue(json);
      }

      virtual void onbeginobject(Match* match) {
         
         ObjectPointer objectPointer = ObjectPointer(new Object());
         Variable object = objectPointer;

         _stack.push(
            object
         );

         BeeFishJSON::JSONParser::onbeginobject(match);
         
      }

      virtual void onobjectvalue(const BString& key, const BeeFishJSON::JSON* parserValue) {

         
         Variable value = _stack.pop();
         
         Variable& object = _stack.top();

         BeeFishBScript::ObjectPointer objectPointer = object;

         (*objectPointer)[key] = value;

         BeeFishJSON::JSONParser::onobjectvalue(key, parserValue);
      }

      virtual void onendobject(Match* match) {
         BeeFishJSON::JSONParser::onendobject(match);
      }  


      virtual void onbeginarray(Match* match) {
         _stack.push(
            ArrayPointer(new Array())
         );
         JSONParser::onbeginarray(match);
      }

      virtual void onarrayvalue(Match* match) {
         Variable value = _stack.pop();
         Variable& array = _stack.top();
         ArrayPointer pointer = array;

         pointer->push_back(value);

         JSONParser::onarrayvalue(match);
      }

      virtual void onendarray(Match* match) {
         JSONParser::onendarray(match);
      }

      Variable createVariable(const BeeFishJSON::JSON* value) {

         switch (value->type()) {
         case BeeFishJSON::Type::UNDEFINED:
            return Variable::Undefined();
         case BeeFishJSON::Type::__NULL:
            return nullptr;
         case BeeFishJSON::Type::BOOLEAN:
            if (value->value() == "true")
               return true;
            else if (value->value() == "false")
               return false;
            else
               throw std::runtime_error("Invalid boolean value");
         case BeeFishJSON::Type::NUMBER:
            {
               std::string str = value->value().str();
               double number =  atof(str.c_str());
               return number;
            }
         case BeeFishJSON::Type::STRING:
            return value->value();
         default:
            throw std::logic_error("Invalid object type");
         }

      }

   };

//   inline void BeeFishBScript::BScriptParser::Object::matchedSetItem(_KeyValue* item);
}

#endif