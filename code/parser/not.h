#ifndef BEE_FISH_PARSER__NOT_H
#define BEE_FISH_PARSER__NOT_H

#include "match.h"

namespace BeeFishParser {

    class Not : public Match {
    protected:
        Match* _item;
    public:

        Not() : Match() {
        }

        Not(Match* match)
        {
            _item = match;
        }
        
        virtual ~Not() {
            delete _item;
        }
        
        virtual void setup(Parser* parser)
        {
            _parser = parser;
            _item->setup(parser);
        }

        virtual bool matchCharacter(const Char& character)
        override
        {
            
            bool matched =
                _item->match(_parser, character);
            
            
            if (!matched || _item->result() == false) {
                success();
                return true;
            }
            else if ( _item->result() == true) {
                fail();
                return false;
            }
            
            return true;
          
        }
        
        virtual void eof(Parser* parser)
        override
        {
            setup(parser);
            
                
            _item->eof(parser);
            
            if (_item->result() == true)
                fail();
            else
                success();
        }
        
        virtual optional<bool> result() const
        override
        {
            if (_item->result() == true)
                return false;
            else if (_item->result() == false)
                return true;
                
            return _result;
        }
        

    };
    
};

#endif
