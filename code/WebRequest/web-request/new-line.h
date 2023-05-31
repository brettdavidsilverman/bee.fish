#ifndef BEE_FISH_WEB__NEW_LINE_H
#define BEE_FISH_WEB__NEW_LINE_H

#include <map>
#include <vector>
#include <typeinfo>
#include "../parser/parser.h"
#include "../json/json-parser.h"

using namespace BeeFishParser;
      
namespace BeeFishWeb {

      class NewLine : public Or {
      public:
         NewLine() :
            Or(
                  new Word("\r\n"),
                  new Word("\n")
            )
         {

         }
      };
      

}

#endif