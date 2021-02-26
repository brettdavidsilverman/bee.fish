#ifndef BEE_FISH_PARSER__NOT_H
#define BEE_FISH_PARSER__NOT_H

#include "match.h"

namespace bee::fish::parser {

   class Not : public Match{
   protected:
      Match* _item;
   public:

      Not(Match* match)
      {
         _item = match;
      }

      virtual ~Not()
      {
         delete _item;
      }
      
      virtual bool match(const Char& character)
      {
      
         bool matched =
            _item->match(character);
            
         //if (!matched)
            Match::match(character);
     
      
         if (_item->result() == false)
            success();
         else if (_item->result() == true)
            fail();
         else if (character == Match::EndOfFile) {
            success();
         }
         
         return !matched;
      
      }
   
      Not(const Not& source)
      {
         _item = source._item->copy();
      }
			   
      virtual Match* copy() const
      {
         return new Not(*this);
      }
      
      virtual void write(
         ostream& out,
         size_t tabIndex = 0
      ) const
      {
         writeHeader(out, "Not", tabIndex);
         out << endl;
         out << tabs(tabIndex) << "(" << endl;
         _item->write(out, tabIndex + 1);
         out << endl;
         out << tabs(tabIndex) << ")";
      }
   
   };

   class NotPtr : public MatchPtr
   {
   public:
      NotPtr(MatchPtr item) :
         MatchPtr(new Not(item->copy()))
      {
      }
   };

};

#endif
