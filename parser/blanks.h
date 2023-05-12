#ifndef BEE_FISH_PARSER__BLANKS_H
#define BEE_FISH_PARSER__BLANKS_H
#include <ostream>
#include <vector>
#include "../misc/optional.h"
#include "match.h"

using namespace std;

namespace BeeFishParser {

    class BlankChar : public Or
    {
    public:
        BlankChar() : Or(
        new BeeFishParser::
            Character(' '),
            
        new BeeFishParser::
            Character('\t')
        )
        {
        }

    };

    class Blanks : public Repeat<BlankChar>
    {
    public:
        Blanks() : Repeat()
        {
        }

    };
    
}

#endif