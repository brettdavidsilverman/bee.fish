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
			   
			  virtual void write(ostream& out, size_t tabIndex = 0) const
      {
         writeHeader(out, "Range", tabIndex);
         out << "('";
         UTF8Character::writeEscaped(
            out, _minimum
         );
         out << "', '";
         UTF8Character::writeEscaped(
            out, _maximum
         );
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
			   
      virtual Match* copy() const
      {
         return new Range(*this);
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
