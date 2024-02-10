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
      Parser* _capture;
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
         _value(""),
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
         delete _capture;
      }

      virtual void capture(char c)
      {
         _valueRef += c;
        // _valueRef.push_back(c);
      }
      
      virtual const std::string& value() const
      override
      {
         return _valueRef;
      }
      
      virtual std::string& value()
      override
      {
         return _valueRef;
      }

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
            *_capture,
            c
         );
         
         if (matched)
            capture(c);
            
         return matched;
      }

      virtual bool flush()
      override
      {
         if (!Parser::flush())
            return false;

         return _capture->flush();
      }



   };

}

#endif