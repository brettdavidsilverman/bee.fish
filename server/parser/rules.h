#ifndef BEE_FISH_PARSER__RULES_H
#define BEE_FISH_PARSER__RULES_H

#include <memory>

#include "parser.h"

using namespace std;

namespace bee::fish::parser {

 
   inline static const CharacterPtr eof(Match::EndOfFile);
   
   MatchPtr operator and (MatchPtr first, MatchPtr second)
   {
      return AndPtr(first, second);
   }

   MatchPtr operator or (MatchPtr first, MatchPtr second)
   {
      return OrPtr(first, second);
   }
    
   MatchPtr operator not (MatchPtr item)
   {
      return NotPtr(item);
   }
  

   MatchPtr operator += (MatchPtr repeat, MatchPtr untill)
   {
      return RepeatPtr(repeat, untill, 0, 0);
   }
   
   MatchPtr OptionalPtr(MatchPtr optional, MatchPtr next)
   {
      return ((optional and next) or next);
   }
   
  
}

#endif