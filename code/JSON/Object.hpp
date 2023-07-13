#ifndef BEE_FISH__JSON__OBJECT_HPP
#define BEE_FISH__JSON__OBJECT_HPP

#include <map>
#include <memory>

#include "../Parser/Parser.hpp"

#include "Number.hpp"
#include "String.hpp"
#include "BlankSpace.hpp"

namespace BeeFishJSON {

   using namespace BeeFishParser;

   Parser* _JSON(Parser* params);

   class Object : public Parser {
   protected:
      Parser* _params = nullptr;
      And _parser;
   public:

      Object(Parser* params = nullptr) :
         _params(params),
         _parser(createParser(params))
      {
      }

      Object(const Object& source) :
         _params(source._params),
         _parser(createParser(_params))
      {
      }

      And createParser(Parser* params) {

         const auto openBrace =
            Character("{");

         const auto closeBrace =
            Character("}");

         const auto key =
            _string;

         const auto colon =
            Character(":");

         const auto value =
            LoadOnDemand(_JSON, params);

         const auto seperator =
             -blankSpaces and
             Character(",") and
             -blankSpaces;

         const auto line =
            key and -blankSpaces and
            colon and -blankSpaces and 
            value and -blankSpaces;

         const auto object =
            -blankSpaces and
            openBrace and -blankSpaces and
            Optional(
               line and
               Repeat(
                  seperator and line,
                  0
               )
            ) and
            closeBrace;

         return object;

      }

      virtual Parser* copy() const
      override
      {
         return new Object(*this);
      }

      virtual bool read(char c)
      override
      {

         if (_parser._result != nullopt)
            return false;

         bool matched = _parser.read(c);

         if (_parser._result != nullopt)
            setResult(_parser._result);

         return matched;
      }


   };


}

#endif