#ifndef BEE_FISH_PARSER__RANGE_H
#define BEE_FISH_PARSER__RANGE_H

#include "match.h"

namespace BeeFishParser {

            
	class Range : public Match {
	private:
		Char _minimum;
		Char _maximum;
			   
	public:
		Range(Char minimum, Char maximum) :
			_minimum(minimum),
			_maximum(maximum)
		{
		}

		virtual bool matchCharacter(const Char& character)
		{
				
			bool matched =
				(_minimum <= character) &&
				(_maximum >= character);
				
			if (matched)
			{
				_result = true;
			}
			else 
			{
				_result = false;
			}

			return matched;
		}

	};
		

};

#endif
