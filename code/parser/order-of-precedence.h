#ifndef BEE_FISH_PARSER__ORDER_OF_PRECEDENCE_H
#define BEE_FISH_PARSER__ORDER_OF_PRECEDENCE_H

#include <vector>
#include "../Miscellaneous/Optional.hpp"
#include "match.h"
#include "or.h"

namespace BeeFishParser {

    class OrderOfPrecedence : public Or {
    public:

        template<typename ...T>
        OrderOfPrecedence(T*... inputs) : 
            Or({inputs...})
        {
        }
        
        virtual bool confirm(vector<Match*>::iterator& it)
        override
        {
            // Check that all higher
            // order of precedence
            // have no result so far
            auto end = distance(_inputs.begin(), it) - 1;

            for (ssize_t index = 0;
                index < end;
                ++index)
            {
                Match* higher = _inputs[index];
                
                if (higher->result() == nullopt)
                    return false;
            }
            
            
            
            return true;
            
        }

    };


};

#endif
