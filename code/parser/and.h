#ifndef BEE_FISH_PARSER__AND_H
#define BEE_FISH_PARSER__AND_H
#include <ostream>
#include <vector>
#include "../Miscellaneous/Optional.hpp"
#include "match.h"

using namespace std;

namespace BeeFishParser {

    class And : public Match {
    protected:
        vector<Match*> _inputs;
        size_t _iterator;
    public:

        template<typename ...T>
        And(T* ... inputs) :
            _inputs{inputs...}
        {
             _iterator = 0;
        }
        
        And(Match& left, Match& right)
        {
            _iterator = 0;
            _inputs.push_back(&left);
            _inputs.push_back(&right);
            _delete = false;
        }
      
        virtual ~And()
        {
            if (_delete) {
                for (auto it : _inputs)
                {
                    Match* match = it;
                    if (match)
                       delete match;
                }
            }
        }

        virtual void setup(Parser* parser) {
            if (_parser)
                return;
            
            _parser = parser;
         
            for (auto item : _inputs) {
                item->setup(parser);
            }
        
            _iterator = 0;
         
        }     

        virtual bool match(Parser* parser, const Char& character)
        override
        {
            setup(parser);
            
            if (result() != nullopt)
                 return false;
                 
            bool matched = false;
            
            if ( _iterator == size() ) {
                fail();
                return false;
            }
            
            while ( !matched &&
                    _result == nullopt )
            {
                Match* item = _inputs[_iterator];

                matched =
                    item->match(_parser, character);
         
                if (item->result() == true)
                {
            
                    if ( ++_iterator == 
                        size() )
                    {
                       // matched = true;
                        success();
                    }
               
                }
                else if (item->result() == false)
                {
                    matched = false;
                    fail();
                }

            
            }

            return matched;
         
        }

        size_t size() {
            return _inputs.size();
        }

        void push_back(Match* match) {
         
            if (_parser)
                match->setup(_parser);

            _inputs.push_back(match);

        }
    
        virtual void eof(Parser* parser) {
          
            if (!_parser)
                setup(parser);
            

            if (result() != nullopt)
                return;
     
   
   
            for (size_t index = _iterator; index < _inputs.size(); ++index)
            {

                Match* match = (*this)[index];
            
                match->eof(parser);
            
                if (match->result() == false)
                {
                    fail();
                    return;
                }
            }
    
            success();
        }
      
        virtual Match* operator[] (size_t index)
        {
            return _inputs[index];
        }
      
      
    };

};

#endif
