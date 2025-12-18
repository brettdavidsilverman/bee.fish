#ifndef BEE_FISH_PARSER__NOT_H
#define BEE_FISH_PARSER__NOT_H

#include "match.h"

namespace BeeFishParser {

    class Not : public Match {
    public:

        Not(Match* match) :
            Match(match)
        {
        }
        

        virtual bool match(Parser* parser, const Char& character)
        override
        {
            setup(parser);
            
            if (_result == true)
                return true;
                
            if (_result == false)
                return false;
            
            bool matched =
                _match->match(parser, character);
            
            if (!matched || _match->result() == false) {
                success();
            }
            else if ( _match->result() == true) {
                fail();
                return false;
            }
            
            return true;
          
        }
        
        virtual void eof(Parser* parser)
        override
        {
            setup(parser);


            if (result() != nullopt)
                return;

            _match->eof(parser);
            
            if (_match->result() == true) {
                fail();
            }
            else
                success();
        }
        
        virtual optional<bool> result() const
        override
        {
            if (_result != nullopt)
                return _result;
                
            if (_match) {
                if (_match->result() == true)
                    return false;
                else if (_match->result() == false)
                    return true;
            }
            
            return nullopt;
        }
        

    };
    
};

#endif
