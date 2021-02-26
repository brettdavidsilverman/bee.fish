#ifndef BEE_FISH_PARSER__RULES_H
#define BEE_FISH_PARSER__RULES_H

#include <memory>

#include "parser.h"

using namespace std;

namespace bee::fish::parser {

 
   inline static const CharacterPtr eof(Match::EndOfFile);
   
   AndPtr operator and (MatchPtr& first, MatchPtr& second)
   {
      return AndPtr(first, second);
   }

   OrPtr operator or (MatchPtr& first, MatchPtr& second)
   {
      return OrPtr(first, second);
   }
    
   NotPtr operator not (MatchPtr& item)
   {
      return NotPtr(item);
   }
  

   RepeatPtr operator ++ (const MatchPtr& repeat)
   {
      return RepeatPtr(repeat);
   }
   
   MatchPtr OptionalPtr(MatchPtr& optional)
   {
      return MatchPtr(new Optional(optional));
   }
   
   MatchPtr OptionalPtr(MatchPtr& optional, MatchPtr& next)
   {
      return ((optional and next) or next);
   }
   
   
   MatchPtr operator - (MatchPtr& optional)
   {
      return OptionalPtr(optional);
   }
   
  
}

#endif