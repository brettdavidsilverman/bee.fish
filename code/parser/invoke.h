#ifndef BEE_FISH_PARSER__INVOKE_H
#define BEE_FISH_PARSER__INVOKE_H

#include <iostream>
#include <string>
#include "../Miscellaneous/Optional.hpp"
#include <map>
#include <sstream>
#include <functional>
#include "match.h"

namespace BeeFishParser {
    
using namespace std;

    class Invoke : public Match
    {
    public:
    
        typedef std::function<bool(Match*)> Function;
        Function _function = nullptr;

    public:
    
        Invoke(Match* match) : Match(match) {

        }

        Invoke(
            Match* match,
            Function func
        ) :
            Match(match),
            _function(func)
        {
        }

        virtual ~Invoke() {
        }

        virtual void success()
        override
        {

            if (_function) {
                if (!_function(_match))
                {
                    fail();
                    return;
                }
            }
            
            Match::success();
            
        }
        
        
        
    };




}

#endif


