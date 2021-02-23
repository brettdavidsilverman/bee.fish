#ifndef BEE_FISH_PARSER__NOT_H
#define BEE_FISH_PARSER__NOT_H

#include "match.h"

namespace bee::fish::parser {

   class Not : public Match
   {
   protected:
      MatchPtr _item;
   public:

      Not(MatchPtr match) :
         _item(match)
      {
      }
   


      virtual bool match(const Char& character)
      {
      
         bool matched =
            _item->match(character);
            
         if (_item->result() == false)
            success();
         else if (_item->result() == true)
            fail();
         else if (character == Match::EndOfFile) {
            success();
         }
         
         if (!matched)
            Match::match(character);
            
         return matched;
      
      }
   
      Not(const Not& source) :
         _item(source._item->copy())
      {
      }
			   
      virtual MatchPtr copy() const
      {
         return MatchPtr(new Not(*this));
      }
      
      virtual void write(ostream& out) const
      {
         out << "Not";
         writeResult(out);
         out << "("
             << *_item
             << ")";
         
      }
   };


};

#endif
