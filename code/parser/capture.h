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
        
        Capture(Match& match) :
            Match(match)
        {
        }
        
        Capture(Match* match, BString& captured)
        {
            _match = new Invoke(
                new Capture(match),
                [&captured, this](Match* match)
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
             
             if (result() != nullopt)
                 return false;
             
             bool matched = _match->match(parser, character);
            
             if (matched)
                 _value += character;
          
             if (_match->result() == true)
                 success();
             else if (_match->result() == false)
                 fail();
                 
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