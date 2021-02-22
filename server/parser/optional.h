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
      Match* _match;
      bool _matched = false;
   
   public:
      Optional(Match* match) :
         Match(),
         _match(match)
      {
      }
   
      virtual ~Optional()
      {
         if (_match)
            delete _match;
      }
   
		   virtual bool match(const Char& character)
		   {
		     
		      bool matched =
		         _match->match(character);
		      
		      
		      if (_match->result() == true)
		      {
		         success();
		         _matched = true;
		      } 
		      else if (_match->result() == false)
		      {
		         matched = false;
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
		      return *_match;
		   }
		   
		   virtual bool isOptional()
		   {
		      return true;
		   }
     
      Optional(const Optional& source) :
         Match(source),
         _match(source._match->copy())
      {
      }
			   
      virtual Match* copy() const
      {
         return new Optional(*this);
      }
      
      virtual void write(ostream& out) const
      {
         out << "Optional";
         writeResult(out);
         out << "("
             << *_match
             << ")";
         
      }
   
   };


}

#endif
