#ifndef BEE_FISH_PARSER__RANGE_H
#define BEE_FISH_PARSER__RANGE_H

#include "match.h"

namespace bee::fish::parser {

            
   class Range : public Match {
   private:
			  WideChar _minimum;
			  WideChar _maximum;
			   
		public:
			  Range(WideChar minimum, WideChar maximum)
			     : Match(),
			     _minimum(minimum),
         _maximum(maximum)
			  {
			  }
			   
			  virtual bool match(WideChar character)
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
			     else {
			        fail();
			     }
			
			     return matched;
			  }
			   
			  virtual void write(ostream& out)
			  {
			     out << "Range";
			     writeResult(out);
			     out << "('";
			     Match::write(out, _minimum);
			     out << "', '";
			     Match::write(out, _maximum);
			     out << "')";
			       
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
