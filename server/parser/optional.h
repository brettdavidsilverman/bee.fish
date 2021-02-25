#ifndef BEE_FISH_PARSER__OPTIONAL_H
#define BEE_FISH_PARSER__OPTIONAL_H
#include <string>
#include <stdexcept>
#include "match.h"

namespace bee::fish::parser {

   using namespace std;

   class Optional : public Match
   {
   protected:
      MatchPtr _item;
   
   public:
      Optional(MatchPtr match) :
         _item(match)
      {
         _optional = true;
      }
   
		   virtual bool match(const Char& character)
		   {
		     
         bool matched = _item->match(character);
		      
		      
		      if (_item->result() == true)
		      {
		         success();
		      } 
		      else if (_item->result() == false)
		      {
		         success();
		      }
		      else if (character == Match::EndOfFile)
		      {
		         success();
		      }
		         
		      if (matched)
		         Match::match(character);
		         
		      return matched;
		   }
      
		   virtual Match& item()
		   {
		      return *_item;
		   }
		   
      virtual bool isMatched()
      {
         return (_item->isMatched());
      }
      
      virtual bool isOptional()
      {
         return true;
      }
      
      Optional(const Optional& source) :
         _item(source._item->copy())
      {
      }
			   
      virtual MatchPtr copy() const
      {
         return MatchPtr(new Optional(*this));
      }
      
      virtual void write(ostream& out) const
      {
         out << "Optional";
         writeResult(out);
         out << "("
             << *_item
             << ")";
         
      }
   
   };


}

#endif
