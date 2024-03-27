#ifndef BEE_FISH_PARSER__CAPTURE_H
#define BEE_FISH_PARSER__CAPTURE_H

#include <iostream>
#include <string>
#include "../misc/optional.h"
#include <sstream>
#include "match.h"

using namespace std;

namespace BeeFishParser {
   
   class Capture : public Match
   {
   public:
      BString _value;
      //BString& _valueRef;
      

   public:
      Capture() :
         Match(),
         _value("")//,
         //_valueRef(_value)
      {
      }
      
      Capture(Match* match) :
         Match(match),
         _value("")
        // _valueRef(_value)
      {
      }
      
      /*
      Capture(
         Match* match,
         BString& value
      ) :
         Match(match),
         _valueRef(value)
      {
         _valueRef.clear();
      }*/
      
      virtual ~Capture() {
      }      

      virtual void capture(Parser* parser, char c)
      override
      {
          if (!_parser)
            setup(parser);
            
          _parser = parser;
          
          if (_match) {
             _match->capture(parser, c);
          }
          
          _value.push_back(c);
          
      }
      
      virtual const BString& value() const
      override
      {
         return _value;
      }

   };

}

#endif