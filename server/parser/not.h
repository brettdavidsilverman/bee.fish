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
         
         if (character == Match::EndOfFile)
            success();
         else if (_item->_result == false)
         {
            _item->reset();
            Match::match(character);
            return true;
         }
         else if (_item->_result == true)
            fail();
         else if (!matched)
         {
            // Reset the item
            _item->reset();
            Match::match(character);
            return true;
         }
         
            
         return false;
      
      }
   
      Not(const Not& source) :
         _item(source._item->copy())
      {
      }
			   
      virtual MatchPtr copy() const
      {
         return MatchPtr(new Not(*this));
      }
      
      virtual void reset()
      {
         Match::reset();
         _item->reset();
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
   
   class NotPtr : public MatchPtr
   {
   public:
      NotPtr(MatchPtr item) :
         MatchPtr(new Not(item))
      {
      }
   };


};

#endif
