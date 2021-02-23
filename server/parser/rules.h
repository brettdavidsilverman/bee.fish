#ifndef BEE_FISH_PARSER__RULES_H
#define BEE_FISH_PARSER__RULES_H

#include <memory>

#include "parser.h"

using namespace std;

namespace bee::fish::parser {

   #define WORD(word) MatchPtr(new Word(word))
   #define C(character) MatchPtr(new Character(character))
   
   MatchPtr operator and (MatchPtr first, MatchPtr second)
   {
      return MatchPtr(new And(first, second));
   }

   MatchPtr operator or (MatchPtr first, MatchPtr second)
   {
      return MatchPtr(new Or(first, second));
   }
    
   MatchPtr operator not (MatchPtr item)
   {
      Not* _not = new Not(item);
      MatchPtr pointer(_not);
      return pointer;
   }
   
   MatchPtr operator ~ (MatchPtr item)
   {
      return MatchPtr(new Optional(item));
      
      //return (item or (not item));
   }
 
  
}

#endif