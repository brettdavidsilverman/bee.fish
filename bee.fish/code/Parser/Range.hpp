#ifndef BEE_FISH_PARSER__RANGE_HPP
#define BEE_FISH_PARSER__RANGE_HPP
#include <memory>

#include "UTF8Character.hpp"

namespace BeeFishParser {

            
	class Range : public UTF8Character {
	protected:
		UTF8Character _minimum;

        UTF8Character _maximum;

	public:
        using UTF8Character::read;

        Range(const Range& source) :
           _minimum(source._minimum),
           _maximum(source._maximum)
        {
        }

		Range( const UTF8Character& minimum,
               const UTF8Character& maximum ) :
			_minimum(minimum),
			_maximum(maximum)
		{
		}

        Range( const std::string& minimum,
               const std::string& maximum ) 
        : Range(UTF8Character(minimum),
                UTF8Character(maximum))
		{
		}

		virtual bool read(
           const UTF8Character& character
        ) override
		{
            using namespace std;

			bool matched =
				(_minimum <= character) &&
				(_maximum >= character);
				
			if (matched)
			{
				setResult(true);
			}
			else 
			{
				setResult(false);
			}

			return matched;
		}

        virtual Parser* copy() const 
        override 
        {
           return new Range(
              *this
           );
        }

	};
		

};

#endif
