#ifndef BEE_FISH_PARSER__RULES_HPP
#define BEE_FISH_PARSER__RULES_HPP

#include "Parser.hpp"
#include "And.hpp"
#include "Or.hpp"
#include "Not.hpp"

namespace BeeFishParser {

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

   auto Optional (
      const Parser& a,
      const Parser& b
   )
   {
      return (a and b) or b;
   }
   

   
};

#endif
