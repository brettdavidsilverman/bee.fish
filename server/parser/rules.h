#ifndef BEE_FISH_PARSER__RULES_H
#define BEE_FISH_PARSER__RULES_H
#include <string>
#include <vector>
#include <iostream>
#include <optional>
#include <memory>

#include "version.h"
#include "match.h"
#include "utf-8.h"
#include "character.h"
#include "range.h"
#include "word.h"
#include "ciword.h"
#include "repeat.h"

#include "or.h"
#include "and.h"
#include "not.h"
/*
#include "optional.h"
#include "load-on-demand.h"
*/

using namespace std;

namespace bee::fish::parser {


   inline Match operator and(const Match& first, const Match& second)
   {
      return And(first, second);
   }
   
   inline Match operator or(const Match& first, const Match& second)
   {
      return Or(first, second);
   }
   
   inline Match operator ~(const Match& match)
   {
      return Optional(match);
   }
   
   inline Match operator not(const Match& match)
   {
      return Not(match);
   }
   

}

#endif