#ifndef BEE_FISH_PARSER__RANGE_H
#define BEE_FISH_PARSER__RANGE_H

#include "match.h"
#include "bstring.h"

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
			         << bstring(_minimum)
			         << "', '"
			         << bstring(_maximum)
			         << "')";
			       
			  }
			  
      Range(const Range& source) :
         _minimum(source._minimum),
         _maximum(source._maximum)
      {
      }
			   
      virtual Match* copy() const
      {
         return new Range(*this);
      }
	 };
		

};

#endif
