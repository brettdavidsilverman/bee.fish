#ifndef BEE_FISH_PARSER__OPTIONAL_H
#define BEE_FISH_PARSER__OPTIONAL_H
#include <string>
#include <stdexcept>
#include "match.h"

namespace BeeFishParser {

   using namespace std;

   class Optional : public Match {
   protected:
      bool _matched = false;

   public:
      Optional() : Match() {
      }

      Optional(Match* match) : Match(match)
      {
      }

      virtual ~Optional() {
      }
   
      virtual bool matchCharacter(const Char& character)
      {
         
         bool matched =
            _match->match(_parser, character);
         
         bool succeeded = false;
         
         if (_match->_result == true)
         {
         
            _matched = true;
            succeeded = true;
            
         } 
         else if (_match->_result == false)
         {
            _matched = false;
            succeeded = true;
         }

         if (succeeded)
            _result = true;

         return matched;
      }
   
   };


}

#endif
