#ifndef BEE_FISH_PARSER__CAPTURE_H
#define BEE_FISH_PARSER__CAPTURE_H

#include <iostream>
#include <string>
#include <sstream>

#include "Parser.hpp"

using namespace std;

namespace BeeFishParser {
   
   class Capture : public Character
   {
   protected:
      std::string _value;
   public:
      std::string& _valueRef;
      
   public:
      Capture() :
         Character(),
         _valueRef(_value) 
      {
      }
      
                  
      Capture(
         const Capture& source
      ) :
         Character(source),
         _value(""),
         _valueRef(source._valueRef)
      {
      }

      Capture(
         const Character& parser,
         std::string& value
      ) :
         Character(parser),
         _valueRef(value)
      {
      }
      
      virtual ~Capture() {
      }

      virtual void capture(char c)
      override
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
/*
      virtual bool read(bool bit)
      override
      {
         throw std::logic_error("Should not be here");
      }
*/


   };

}

#endif