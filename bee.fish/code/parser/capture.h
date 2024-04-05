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

   public:
      Capture() :
         Match()
      {
      }
      
      Capture(Match* match) :
         Match(match)
      {
      }
      
      virtual ~Capture() {
      }      

      virtual void capture(Parser* parser, char c)
      override
      {
          //cerr << "'" << c << "'" << flush;
          
          setup(parser);
            
          _value.push_back(c);
          
          Match::capture(parser, c);
         
          
          
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
      
      virtual void eof(Parser* parser)
      override
      {
          if (!_parser)
             setup(parser);
             
          _parser = parser;
             
          if (_match && _match->result() == nullopt)
          {
             _match->eof(_parser);
             
             if (_match->result() == true) {
                success();
             }
             else if (_match->result() == false)
             {
                fail();
             }
          }
      }

   };

}

#endif