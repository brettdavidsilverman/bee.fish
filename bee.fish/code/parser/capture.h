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
      BString& _valueRef;
      BString _value;

   public:
      Capture() :
         Match(),
         _valueRef(_value) 
      {
      }
      
      Capture(Match* match) :
         Match(match),
         _valueRef(_value)
      {
      }
            
      Capture(
         Match* match,
         BString& value
      ) :
         Match(match),
         _valueRef(value)
      {
      }
      
      virtual ~Capture() {
      }      

      virtual void capture()
      {
         _valueRef.push_back(_match->character());
      }
      
      virtual BString value()
      {
         return _valueRef;
      }

   };

}

#endif