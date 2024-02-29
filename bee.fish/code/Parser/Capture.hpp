#ifndef BEE_FISH_PARSER__CAPTURE_HPP
#define BEE_FISH_PARSER__CAPTURE_HPP

#include <iostream>
#include <string>
#include <sstream>
#include <memory>
#include "../Miscellaneous/Miscellaneous.hpp"
#include "ParserBase.hpp"

using namespace std;

namespace BeeFishParser {
   
   using namespace BeeFishMisc;

   class Capture : public Parser
   {
   protected:
      Parser* _parser;
      
   public:
      std::string _value;
      std::string& _valueRef;
      


   public:
      using Parser::read;

      Capture(
         const Capture& source
      ) :
         Parser(),
         _parser(source._parser->copy()),
         _value(""),
         _valueRef(source._valueRef)

      {
      }

      Capture(
         const Parser& parser
      ) :
         _parser(parser.copy()),
         _value(""),
         _valueRef(_value)
      {
      }

      Capture(
         const Parser& parser,
         std::string& value
      ) :
         _parser(parser.copy()),
         _value(""),
         _valueRef(value)
      {
      }
      
      virtual ~Capture() {
         delete _parser;
      }

      virtual const std::string& value() const
      override
      {
         return _valueRef;
      }
/*
      virtual std::string& value()
      override
      {
         return _valueRef;
      }
*/
      virtual Parser* copy() const
      override
      {
         return new Capture(*this);
      }

      virtual bool read(char c)
      override
      {
         Parser::read(c);

         bool matched = readIndirect(
            *_parser,
            c
         );
         
         if (matched) {
             capture(c);
            _valueRef += c;
         }
         
         return matched;
      }
      
      virtual void capture(char c)
      {
      }

      virtual bool flush()
      override
      {
         if (!Parser::flush())
            return false;

         return _parser->flush();
      }



   };

}

#endif