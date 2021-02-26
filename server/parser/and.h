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
   
      vector<Match*>::iterator
         _iterator;

   public:

      template<typename ...T>
      And(T*... inputs) :
         Match(inputs...)
      {
         _iterator = _inputs.end();
      }
      
      virtual bool
      match(const Char& character) {
      
         bool matched = false;
         vector<Match*>::iterator 
            end = _inputs.end();
            
         if (_iterator == end)
            _iterator = _inputs.begin();
            
         for (;;) {

            Match* item = *_iterator;

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
   
      And(const And& source) :
         Match(source)
      {
			     _iterator = _inputs.end();
      }
			   
      virtual Match* copy() const
      {
         return new And(*this);
      }
      
      virtual void write(
         ostream& out,
         size_t tabIndex = 0
      ) const
      {
         writeHeader(out, "And", tabIndex);
         out << endl;
         writeInputs(out, tabIndex);
      }
      
   };
   
   class AndPtr : public MatchPtr
   {
   public:
      AndPtr(MatchPtr first, MatchPtr second) :
         MatchPtr(
            new And(
               first->copy(),
               second->copy()
            )
         )
      {
      }
   };

		
};

#endif
