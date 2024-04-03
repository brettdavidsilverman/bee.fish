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
      
      /*
          
         if (_result != nullopt)
         {
            //return _result == true;

         }

         bool matched = true;
         Match* _not = _match;
         
         if (_not->result() == nullopt)
         {
             
            matched =
               _not->match(_parser, character);
            cerr << "_not->result()" << _not->result() << endl;
            cerr << "matched " << matched << endl;
         }

         if (_not->result() == false)
         {
            _result = true;
            cerr << "set true" << endl;
 
            return true;
         }
         else if (_not->result() == true) 
         {
            _result = false;
            cerr << "set false" << endl;
            
            return false;
         }
         else if (_not->result() == nullopt)
         {
            return true;
         }

         

         return !matched;
         */
        
      }
      
      virtual const optional<bool>& result() const
      override
      {
         return _result;
      }
      

   };
   
};

#endif
