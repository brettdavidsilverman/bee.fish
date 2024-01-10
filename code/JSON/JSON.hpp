#ifndef BEE_FISH__JSON_H
#define BEE_FISH__JSON_H

#include "../Parser/Parser.hpp"
#include "../PowerEncoding/PowerEncoding.hpp"
#include "Version.hpp"
#include "Map.hpp"
#include "Array.hpp"
#include "Number.hpp"
#include "String.hpp"
#include "BlankSpace.hpp"

namespace BeeFishJSON {

   using namespace BeeFishParser;

   enum class Type {
      UNDEFINED,
      NULL_,
      BOOLEAN,
      NUMBER,
      STRING,
      KEY,
      ARRAY,
      MAP,
      ROOT
   };

   ostream& operator << (ostream& out, const Type& type)
   {
      switch (type)
      {
         case Type::UNDEFINED:
            out << "undefined";
            break;
         case Type::NULL_:
            out << "null";
            break;
         case Type::BOOLEAN:
            out << "bool";
            break;
         case Type::NUMBER:
            out << "number";
            break;
         case Type::STRING:
            out << "string";
            break;
         case Type::ARRAY:
            out << "array";
            break;
         case Type::MAP:
            out << "map";
            break;
         default:
            out << "UNKNOWN";
      }

      return out;
   }

   PowerEncoding& operator << (PowerEncoding& out, const Type& type)
   {
      out << (int)type;
      return out;
   }

   PowerEncoding& operator >> (PowerEncoding& in, Type& value)
   {
      int type;
      in >> type;
      value = (Type)type;
      return in;
   }

   class JSON : public Parser
   {
   protected:
      Parser* _params {nullptr};
      And _parser;

   public:
      JSON(Parser* params = nullptr) :
         _params(params),
         _parser(createParser(params))
      {

      }

      JSON(const JSON& source) :
          _params(source._params),
          _parser(createParser(_params))
      {
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
      

         static const auto undefined =
            Word("undefined");

         static const auto _null =
            Word("null");

         static const auto _true =
            Word("true");

         static const auto _false =
            Word("false");

         static const auto boolean =
            _true or _false;

         return
            blankSpaces and
            (
               undefined or
               _null or
               boolean or
               number or
               _string or
               array or
               Map(params)
            );
      }

   };

   Parser* _JSON(Parser* params) {
      return JSON(params).copy();
   }

}


#endif