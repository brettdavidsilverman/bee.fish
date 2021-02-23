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
      MatchPtr _first;
      MatchPtr _second;
      
   public:

      And(MatchPtr first, MatchPtr second) :
         _first(first),
         _second(second)
      {
      }
      
      And(const And& source) :
         _first(source._first->copy()),
         _second(source._second->copy())
      {
      }
      
      virtual bool match(const Char& character)
      {
      
         bool matched = false;
         
         if ( _first->result() == nullopt )
         {
            matched = _first->match(character);
         }
         
         if ( !matched && _first->result() == true )
         {
         
            if ( _second->result() == nullopt )
            {
               matched = _second->match(character);
            }
         
            if ( _second->result() == true )
               success();
         }
        
         if ( _first->result() == false || 
              _second->result() == false )
            fail();
         
        
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
         out << "(" 
             << *_first
             << ", "
             << *_second
             << ")";
         
      }
      
      virtual Match& operator[]
      (size_t index)
      {
         if (index == 0)
            return *_first;
         else if (index == 1)
            return *_second;

         throw runtime_error("Invalid index");
      }
      
   };

};

#endif
