#ifndef BEE_FISH_PARSER__RULES_HPP
#define BEE_FISH_PARSER__RULES_HPP

#include "Parser.hpp"
#include "Parser.hpp"
#include "And.hpp"
#include "Or.hpp"
#include "Not.hpp"

namespace BeeFishParser {


   auto operator and (
      const Parser& a,
      const Parser& b
   )
   {
      return And(a, b);
   }

   auto operator or (
      const Parser& a,
      const Parser& b
   )
   {
      return Or(a, b);
   }

   auto operator not (
      const Parser& a
   )
   {
      return Not(a);
   }
   

   
};

#endif
