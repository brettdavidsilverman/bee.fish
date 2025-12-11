#ifndef BEE_FISH_WEB__NEW_LINE_H
#define BEE_FISH_WEB__NEW_LINE_H

#include <map>
#include <vector>
#include <typeinfo>
#include "../parser/parser.h"
#include "../json/json-parser.h"


namespace BeeFishWeb {

    using namespace BeeFishParser;
      
    class NewLine : public Or {
    public:
        NewLine() :
            Or(
                new And(
                    new Character("\r"),
                    new Character("\n")
                ),
                new Character("\n")
            )
        {

        }
    };

}

#endif