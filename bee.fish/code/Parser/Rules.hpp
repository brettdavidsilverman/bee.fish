#ifndef BEE_FISH_PARSER__RULES_HPP
#define BEE_FISH_PARSER__RULES_HPP

#include "ParserBase.hpp"

namespace BeeFishParser {

   And operator and (
      const And& a,
      const And& b
   )
   {
      return And(a, b);
   }

   And operator and (
      const And& a,
      const Parser& b
   )
   {
      return And(a, b);
   }

   And operator and (
      const Parser& a,
      const Parser& b
   )
   {
      return And(a, b);
   }

   Or operator or (
      const Or& a,
      const Or& b
   )
   {
      return Or(a, b);
   }

   Or operator or (
      const Or& a,
      const Parser& b
   )
   {
      return Or(a, b);
   }

   Or operator or (
      const Parser& a,
      const Parser& b
   )
   {
      return Or(a, b);
   }

   Not operator not (
      const Parser& a
   )
   {
      return Not(a);
   }

   Optional operator -(
      const Parser& a
   )
   {
      return Optional(a);
   }
   
};

#endif
