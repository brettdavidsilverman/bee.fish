#ifndef BEE_FISH_PARSER__BLANKS_H
#define BEE_FISH_PARSER__BLANKS_H
#include <ostream>
#include <vector>
#include "../Miscellaneous/Optional.hpp"
#include "match.h"

using namespace std;

namespace BeeFishParser {

    class BlankCharacter : public Or
    {
    public:
        BlankCharacter() : Or(
        new BeeFishParser::
            Character(' '),
            
        new BeeFishParser::
            Character('\t')
        )
        {
        }

    };

    class Blankspaces: public Repeat<BlankCharacter>
    {
    public:
        Blankspaces(Size minimum = 0) : Repeat(minimum)
        {
        }

    };
    
}

#endif