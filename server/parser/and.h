#ifndef BEE_FISH_PARSER__AND_H
#define BEE_FISH_PARSER__AND_H
#include <ostream>
#include <vector>
#include <optional>
#include "match.h"

using namespace std;

namespace bee::fish::parser
{

   class And : public Match
   {
   protected:
      vector<MatchPtr> _inputs;
      
      vector<MatchPtr>::iterator
         _iterator;
      
   public:

      template<typename ...T>
      And(T... args) :
         _inputs{args...}
      {
         _iterator = _inputs.end();
      }
      
      And(const And& source) :
         _inputs(source._inputs)
      {
			     _iterator = _inputs.end();
      }
      
      virtual ~And() {
      }
      
      
      virtual bool match(const Char& character)
      {
      
         bool matched = false;
         vector<MatchPtr>::iterator 
            end = _inputs.end();
  
         if (_iterator == end)
            _iterator = _inputs.begin();
            
         for (;;) {

            MatchPtr item = *_iterator;

            matched =
               item->match(character);
               
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
         
         if (matched)
            Match::match(character);
            
         return matched;
      }
			   
      virtual MatchPtr copy() const
      {
         return MatchPtr(new And(*this));
      }
      
      virtual void write(ostream& out) const
      {
         out << "And";
         writeResult(out);
         out << "(";
         writeInputs(out, _inputs);
         out << ")";
         
      }
      
      virtual Match& operator[]
      (size_t index)
      {
         return *(_inputs[index]);
      }
      
   };

};

#endif
