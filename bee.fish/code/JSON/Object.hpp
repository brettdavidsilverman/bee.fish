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

   const auto openBrace =
      Character("{");

   const auto closeBrace =
      Character("}");

   const auto key =
      _string;

   const auto colon =
      Character(":");

   const auto seperator =
      Character(",");

   const auto value =
            LoadOnDemand(_JSON);

   const auto keyValue =
      blankSpaces and
      key and blankSpaces and
      colon and blankSpaces and 
      value and blankSpaces;
            
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

         const auto object =
            blankSpaces and
            openBrace and blankSpaces and
            Optional(
               keyValue and
               Repeat(
                  seperator and keyValue,
                  0
               )
            ) and
            blankSpaces and
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

         return readIndirect(
            _parser,
            c
         );
         
      }


   };


}

#endif