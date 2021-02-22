#ifndef BEE_FISH_PARSER__RANGE_H
#define BEE_FISH_PARSER__RANGE_H

#include "match.h"
#include "b-string.h"

namespace bee::fish::parser {

            
   class Range : public Match {
   private:
			  const Char _minimum;
			  const Char _maximum;
			   
		public:
			  Range(const Char& minimum, const Char& maximum)
			     : Match(),
			     _minimum(minimum),
         _maximum(maximum)
			  {
			  }
			   
			  virtual bool match(const Char& character)
			  {
			   
			     if (character == Match::EndOfFile)
			        return false;
			         
			     bool matched =
			        (_minimum <= character) &&
			        (_maximum >= character);
			         
			     if (matched)
			     {
			        Match::match(character);
			        success();
			     }
			     else
			     {
			        fail();
			     }
			
			     return matched;
			  }
			   
			  virtual void write(ostream& out) const
			  {
			     out << "Range";
			     writeResult(out);
			     out << "('"
			         << BString(_minimum)
			         << "', '"
			         << BString(_maximum)
			         << "')";
			       
			  }
			  
      Range(const Range& source) :
         _minimum(source._minimum),
         _maximum(source._maximum)
      {
      }
			   
      virtual MatchPtr copy() const
      {
         return MatchPtr(new Range(*this));
      }
	 };
		

};

#endif
