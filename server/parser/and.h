#ifndef BEE_FISH_PARSER__AND_H
#define BEE_FISH_PARSER__AND_H
#include <ostream>
#include <vector>
#include <optional>
#include "match.h"

using namespace std;

namespace bee::fish::parser {

   class And;
   
   class And : public Match {
   protected:
   
      vector<MatchPtr>::iterator
         _iterator;

   public:

      template<typename ...MatchPtr>
      And(bool variadic, MatchPtr... inputs) :
         Match(variadic, inputs...)
      {
         _iterator = _inputs.end();
      }
      
      virtual bool
      match(const Char& character) {
      
         bool matched = false;
         vector<MatchPtr>::iterator 
            end = _inputs.end();
            
         if (_iterator == end)
            _iterator = _inputs.begin();
            
         for (;;) {

            MatchPtr item = *_iterator;

            matched =
               item->match(character);
         
            if (matched)
               Match::match(character);

            if (item->result() == true) {
            
               if ( ++_iterator == end ) {
                  success();
               }
               
            }
            else if (item->result() == false) {
            
               fail();
               
            }
            
            if ( matched ||
                 result() != nullopt )
               break;
         }

         return matched;
      }
      
      virtual string name()
      {
         return "And";
      }
   
      And(const And& source) :
         Match(source)
      {
			     _iterator = _inputs.end();
      }
			   
      virtual MatchPtr copy() const
      {
         return MatchPtr(new And(*this));
      }
   };
   
   class AndPtr : public MatchPtr
   {
   public:
      template<typename ...T>
      AndPtr(T... inputs) :
         MatchPtr(new And(true, inputs...))
      {
      }
   };

		
};

#endif
