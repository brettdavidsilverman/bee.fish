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
        Function _onsuccess = nullptr;
        Function _onfail = nullptr;
    public:
    
        Invoke(
            Match* match,
            Function onsuccess = nullptr,
            Function onfail = nullptr
        ) :
            Match(match),
            _onsuccess(onsuccess),
            _onfail(onfail)
        {
        }

        virtual ~Invoke() {
        }

        virtual void success()
        override
        {

            if (_onsuccess) {
                if (!_onsuccess(_match))
                {
                    fail();
                    return;
                }
            }
            
            Match::success();
            
        }
        
        virtual void fail()
        override
        {

            if (_onfail) {
                _onfail(_match);
            }
            
            Match::fail();
            
        }
        
        
        
    };




}

#endif


