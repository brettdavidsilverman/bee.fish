#ifndef BEE_FISH_PARSER__OPTIONAL_H
#define BEE_FISH_PARSER__OPTIONAL_H
#include <string>
#include <stdexcept>
#include "match.h"

namespace bee::fish::parser {

   using namespace std;

   class Optional : public Match {
   protected:
      Match* _item;
      bool _matched = false;
   
   public:
      Optional(Match* match) :
         _item(match)
      {
      }
      
      virtual ~Optional()
      {
         delete _item;
      }
      
		   virtual bool match(const Char& character)
		   {
		     
		      bool matched =
		         item().match(character);
		      
		      
		      if (item().result() == true) {
		         success();
		         _matched = true;
		      } 
		      else if (item().result() == false) {
		         success();
		      }
		      else if (character == Match::EndOfFile) {
		         success();
		      }
		      
		      
		      return matched;
		   }
      
      virtual bool& matched()
		   {
		      return _matched;
		   }
		   
		   virtual Match& item()
		   {
		      return *_item;
		   }
		   
		   virtual BString& value()
		   {
		      if (result() == true)
		         return item()._value;
		         
		      return Match::_value;
		   }
		   
		   virtual bool isOptional()
		   {
		      return true;
		   }
     
      Optional(const Optional& source) 
      {
         _item = source._item->copy();
      }
			   
      virtual Match* copy() const
      {
         return new Optional(*this);
      }
      
      virtual void write(
         ostream& out,
         size_t tabIndex = 0
      ) const
      {
         writeHeader(out, "Optional", tabIndex);
         out << endl;
         out << tabs(tabIndex) << "(" << endl;
         _item->write(out, tabIndex + 1);
         out << endl;
         out << tabs(tabIndex) << ")";
      }
   };
/*
   class Optional2 : public Or
   {
   protected:
      Match* _match;
      Match* _next;
   public:
      Optional2(Match* match, Match* next) :
         Or(
            new And(match, next),
            next->copy()
         )
      {
         _match = match;
         _next = next;
      }
      
      Optional2(const Optional2& source) :
         Optional2(
            source._match->copy(),
            source._next->copy()
         )
      {
      }
      
      virtual ~Optional2()
      {
      }
      
      virtual Match* copy() const
      {
         return new Optional2(*this);
      }
      
      virtual void write(
         ostream& out,
         size_t tabIndex = 0
      ) const
      {
         writeHeader(out, "Optional2", tabIndex);
         out << endl;
         out << tabs(tabIndex) << "(" << endl;
         _match->write(out, tabIndex + 1);
         out << "," << endl;
         _next->write(out, tabIndex + 1);
         out << endl;
         out << tabs(tabIndex) << ")";
      }
      
      
   
   };
  */
   class OptionalPtr : public MatchPtr
   {
   public:
      OptionalPtr(MatchPtr item) :
         MatchPtr(new Optional(item->copy()))
      {
      }
   };
   

}

#endif
