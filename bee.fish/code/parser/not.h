#ifndef BEE_FISH_PARSER__NOT_H
#define BEE_FISH_PARSER__NOT_H

#include "match.h"

namespace BeeFishParser {

   class Not : public Match {
   protected:
      
   public:

      Not() : Match() {
      }

      Not(Match* match)
         : Match(match)
      {
      }
      
      virtual ~Not() {
      }

      virtual bool matchCharacter(const Char& character)
      override
      {
         
         bool matched =
            _match->match(_parser, character);
         
         
         if (_match->result() == false)
            _result = true;
         else if (_match->result() == true)
            _result = false;
         
         return !matched;
        
      }
      
      virtual optional<bool> result() const
      override
      {
         return _result;
      }
      

   };
   
};

#endif
