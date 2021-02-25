#ifndef BEE_FISH_PARSER__RULES_H
#define BEE_FISH_PARSER__RULES_H

#include <memory>

#include "parser.h"

using namespace std;

namespace bee::fish::parser {

   #define WORD(word) MatchPtr(new Word(word))
   #define C(character) MatchPtr(new Character(character))
   #define eof C(Match::EndOfFile)
   #define AND(first, second) MatchPtr(new And(first, second))
   #define OR(first, second) MatchPtr(new Or(first, second))
   #define NOT(match) MatchPtr(new Not(match))
   #define OPTIONAL(optional, next) ((optional and next) or next)

   MatchPtr operator and (MatchPtr first, MatchPtr second)
   {
      return AND(first, second);
   }

   MatchPtr operator or (MatchPtr first, MatchPtr second)
   {
      return OR(first, second);
   }
    
   MatchPtr operator not (MatchPtr item)
   {
      return NOT(item);
   }
  
   MatchPtr operator - (MatchPtr item)
   {
      return OPTIONAL(item, eof);
   }
  
}

#endif