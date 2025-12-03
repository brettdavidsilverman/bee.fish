#ifndef BEE_FISH_PARSER__ORDER_OF_PRECEDENCE_H
#define BEE_FISH_PARSER__ORDER_OF_PRECEDENCE_H

#include <vector>
#include "../Miscellaneous/Optional.hpp"
#include "match.h"
#include "or.h"

namespace BeeFishParser {

    class OrderOfPrecedence : public Or {
        typedef vector<vector<Match*> > Input;
        
    public:
        Input _input;
    public:

        OrderOfPrecedence(Input input) :
            Or(),
            _input(input)
        {
            for (vector<Match*>& order : input) {
                
                for (auto item : order) {
                    _inputs.push_back(item);
                }
            }

        }
        
        ssize_t findGroupIndex(Match* match)
        {
            for (Size index = 0;
                 index < _input.size();
                ++index)
            {
                vector<Match*>& items = _input[index];
                
                for (Match* item : items) {
                    if (item == match)
                        return index;
                }
            }
            
            return -1;
            
        }
        
        virtual bool confirm(Match* match)
        override
        {
            ssize_t groupIndex = findGroupIndex(match);
            
            // Check that all higher
            // order of precedence
            // have no result so far
            for (ssize_t index = groupIndex - 1;
                index >= 0;
                --index)
            {
                vector<Match*>& items = _input[index];
                
                for (Match* higher : items) {
                    
                    if (higher->result() == nullopt)
                        return false;
                }
            }
            

            return true;
            
        }
        

    };


};

#endif
