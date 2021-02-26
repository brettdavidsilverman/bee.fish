#ifndef BEE_FISH_PARSER__RANGE_H
#define BEE_FISH_PARSER__RANGE_H

#include "match.h"

namespace bee::fish::parser {

            
   class Range : public Match {
   private:
			  Char _minimum;
			  Char _maximum;
			   
		public:
			  Range(
			     const Char& minimum,
			     const Char& maximum
			  ) :
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
			     else {
			        fail();
			     }
			
			     return matched;
			  }
			   
			  virtual void write(ostream& out)
			  {
			     out << "Range(";
			     Match::write(out);
			     out << ":'";
			     Match::write(out, _minimum);
			     out << "','";
			     Match::write(out, _maximum);
			     out << "')";
			       
			  }
			   
			  virtual std::string name()
			  {
			     return "Range";
			  }
			  
			  Range(const Range& source) 
      {
         _minimum = source._minimum;
         _maximum = source._maximum;
      }
			   
      virtual MatchPtr copy() const
      {
         return MatchPtr(new Range(*this));
      }
	 };
	 
	 
	 class RangePtr : public MatchPtr
	 {
	 public:
	    RangePtr(
			     const Char& minimum,
			     const Char& maximum
			 ) : MatchPtr(new Range(minimum, maximum))
			 {
			 }
	 };
		

};

#endif
