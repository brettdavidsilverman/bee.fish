#ifndef BEE_FISH_PARSER__RANGE_HPP
#define BEE_FISH_PARSER__RANGE_HPP
#include <memory>

#include "Character.hpp"

namespace BeeFishParser {

            
	class Range : public Character {
	protected:
		std::shared_ptr<Character>
           _minimum;

        std::shared_ptr<Character>
           _maximum;

	public:
        using Character::read;

		Range( const Character& minimum,
               const Character& maximum ) :
			_minimum((Character*)minimum.copy()),
			_maximum((Character*)maximum.copy())
		{
		}

        Range( const std::string& minimum,
               const std::string& maximum ) 
        : Range(Character(minimum),
                Character(maximum))
		{
		}

		virtual bool read(
           const Character& character
        ) override
		{
            using namespace std;

			bool matched =
				(*_minimum <= character) &&
				(*_maximum >= character);
				
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
              *_minimum,
              *_maximum
           );
        }

	};
		

};

#endif
