#ifndef BEE_FISH_PARSER__OPTIONAL_H
#define BEE_FISH_PARSER__OPTIONAL_H
#include <string>
#include <stdexcept>
#include "match.h"

namespace BeeFishParser {

    using namespace std;

    class Optional : public Match {
    public:
        bool _matched = false;
        
    public:
        Optional() : Match() {
        }

        Optional(Match* match) : Match(match)
        {
        }

        virtual ~Optional() {
        }
        
        virtual bool isOptional() const
        override
        {
            return true;
        }
    
        virtual bool match(Parser* parser, const Char& character)
        {
            setup(parser);
            
            _matched = false;
            
            bool matched =
                _match->match(_parser, character);
            
            bool succeeded = false;
            
            if (_match->result() == true)
            {
            
                _matched = true;
                succeeded = true;
                
            } 
            else if (_match->result() == false)
            {
                _matched = false;
                succeeded = true;
            }

            if (succeeded)
                success();

            return matched;
        }
        
        virtual void eof(Parser* parser)
        {
            setup(parser);
            
            if (result() != nullopt)
                return;
                
            _match->eof(parser);
            
            success();

        }

    };


}

#endif
