#ifndef BEE_FISH_PARSER__CAPTURE_H
#define BEE_FISH_PARSER__CAPTURE_H

#include <iostream>
#include <string>
#include "../Miscellaneous/Optional.hpp"
#include <sstream>
#include "match.h"
#include "invoke.h"

using namespace std;

namespace BeeFishParser {
   
   class Capture : public Match
   {
   public:
      BString _value;

   public:
      Capture() :
         Match()
      {
      }
      
      Capture(Match* match) :
         Match(match)
      {
      }
      
      Capture(Match* match, BString& captured)
      {
         _match = new Invoke(
            match,
            [&captured](Match* match)
            {
               captured = match->value();
               return true;
            }
         );
               
      }
      
      virtual ~Capture() {
      }      
      
      virtual bool match(
         Parser* parser,
         const Char& character
      )
      {
         setup(parser);

         bool matched = matchCharacter(character);
         
         if (matched)
            _value += character;
        
         return matched;
      }
      
      virtual const BString& value() const
      override
      {
         return _value;
      }
      
      virtual BString& value()
      override
      {
         return _value;
      }
      
      
   };

}

#endif