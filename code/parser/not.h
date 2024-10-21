#ifndef BEE_FISH_PARSER__NOT_H
#define BEE_FISH_PARSER__NOT_H

#include "match.h"

namespace BeeFishParser {

   class Not : public Match {
   protected:
      Match* _item;
   public:

      Not() : Match() {
      }

      Not(Match* match)
        // : Match(match)
      {
         _item = match;
      }
      
      virtual ~Not() {
         delete _item;
      }
      
      virtual void setup(Parser* parser)
      {
         Match::setup(parser);
         _item->setup(parser);
      }

      virtual bool matchCharacter(const Char& character)
      override
      {
         
         bool matched =
            _item->match(_parser, character);
         
         
         if (_item->result() == false)
            success();
         else if (_item->result() == true)
            fail();
         
         return !matched;
        
      }
      

   };
   
};

#endif
