#ifndef BEE_FISH__SCRIPT_PARSER_H
#define BEE_FISH__SCRIPT_PARSER_H

#include "../json/json.h"
#include "../json/json-parser.h"

#include "Type.hpp"
#include "Variable.hpp"

namespace BeeFishScript
{
   
   using namespace BeeFishBString;
   using namespace BeeFishJSON;
   
#ifdef SERVER
//   typedef BeeFishDatabase::Path<BeeFishPowerEncoding::PowerEncoding> Path;
#endif

   class ScriptParser : public BeeFishJSON::JSONParser
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


      ScriptParser(Match& match) :
         JSONParser(match)
      {
      }
      
      virtual ~ScriptParser()
      {
      }
      
      virtual bool isBScriptParser() {
         return true;
      }

      virtual bool matched() {
         return _match->matched();
      }

      Variable& value() {
         return _stack.root();
      }

      Variable& json() {
         return _stack.root();
      }


      virtual void onvalue(BeeFishJSON::JSON* json) {

         switch (json->type()) {
            case Type::OBJECT:
               if (_stack.size() == 0)
                  _stack.push(ObjectPointer(new Object));
               break;
            case Type::ARRAY:
               if (_stack.size() == 0)
                  _stack.push(ArrayPointer(new Array));
               break;
            default:
               Variable value = createVariable(json);
               _stack.push(value);
         }

         BeeFishJSON::JSONParser::onvalue(json);
      }

      virtual void onbeginobject(JSON* match)
      override
      {
         
         ObjectPointer objectPointer = ObjectPointer(new Object());
         Variable object = objectPointer;

         _stack.push(
            object
         );

         BeeFishJSON::JSONParser::onbeginobject(match);
         
      }

      virtual void onobjectvalue(
         BeeFishJSON::Object* object,
         ObjectKey* key,
         JSON* value
      ) override
      {
         Variable _value = _stack.pop();
         
         Variable& _object = _stack.top();

         BeeFishScript::ObjectPointer objectPointer = _object;

         (*objectPointer)[key->value()] = _value;

         BeeFishJSON::JSONParser::onobjectvalue(object, key, value);
      }

      virtual void onendobject(JSON* match) {
         BeeFishJSON::JSONParser::onendobject(match);
      }  


      virtual void onbeginarray(JSON* match) {
         _stack.push(
            ArrayPointer(new Array())
         );
         JSONParser::onbeginarray(match);
      }

      virtual void onarrayvalue(JSON* match) {
         Variable value = _stack.pop();
         Variable& array = _stack.top();
         ArrayPointer pointer = array;

         pointer->push_back(value);

         JSONParser::onarrayvalue(match);
      }

      virtual void onendarray(JSON* match) {
         JSONParser::onendarray(match);
      }

      Variable createVariable(const BeeFishJSON::JSON* json) {

         switch (json->type()) {
         case Type::UNDEFINED:
            return Variable::Undefined();
         case Type::NULL_:
            return nullptr;
         case Type::BOOLEAN:
            if (json->value() == "true")
               return true;
            else if (json->value() == "false")
               return false;
            else
               throw std::runtime_error("Invalid boolean value");
         case Type::INTEGER:
            {
               std::string str = json->value().str();
               Integer integer =  atol(str.c_str());
               return Variable(integer);
            }
         case Type::NUMBER:
            {
               std::string str = json->value().str();
               long double number =  atof(str.c_str());
               return Variable(number);
            }
         case Type::STRING:
            return json->value();
         default:
            {
               std::stringstream stream;
               stream << "Invalid variable type " << toChar(json->type());
               
               throw std::logic_error(stream.str());
            }
         }

      }

   };

//   inline void BeeFishScript::ScriptParser::Object::matchedSetItem(_KeyValue* item);
}

#endif