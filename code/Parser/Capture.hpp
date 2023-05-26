#ifndef BEE_FISH_PARSER__CAPTURE_HPP
#define BEE_FISH_PARSER__CAPTURE_HPP

#include <iostream>
#include <string>
#include <sstream>
#include <memory>

#include "ParserBase.hpp"

using namespace std;

namespace BeeFishParser {
   
   class Capture : public Parser
   {
   protected:
      std::shared_ptr<Parser> _capture;
      std::string _value;
   public:
      std::string& _valueRef;
      

   public:
      using Parser::read;

      Capture(
         const Capture& source
      ) :
         Parser(),
         _capture(source._capture->copy()),
         _value(""),
         _valueRef(source._valueRef)
      {
      }
      
      Capture(
         const Parser& parser
      ) :
         _capture(parser.copy()),
         _valueRef(_value)
      {
      }

      Capture(
         const Parser& parser,
         std::string& value
      ) :
         _capture(parser.copy()),
         _valueRef(value)
      {
      }
      
      virtual ~Capture() {
      }

      virtual void capture(char c)
      {
         _valueRef.push_back(c);
      }
      
      virtual const std::string& value() const
      override
      {
         return _valueRef;
      }

      virtual Parser* copy() const
      override
      {
         return new Capture(*this);
      }

      virtual bool read(bool bit)
      override
      {
         throw std::logic_error("Capture::read(bool) 🚫");
      }

      virtual bool read(char c)
      override
      {
         bool matched = _capture->read(c);
         if (_capture->_result == true)
            setResult(true);
         else if (_capture->_result == false)
            setResult(false);

         if (matched)
            capture(c);

         return matched;
      }



   };

}

#endif