#ifndef BEE_FISH_PARSER__REPEAT_HPP
#define BEE_FISH_PARSER__REPEAT_HPP

#include <vector>
#include <memory>

#include "Parser.hpp"

namespace BeeFishParser
{

	class Repeat : public Character
	{
	protected:
		std::shared_ptr<Parser> _template;
        Parser* _item {nullptr};

	public:
		size_t _minimum = 1;
		size_t _maximum = 0;
		size_t _matchedCount = 0;

	public:
        using Parser::read;

		Repeat(
            const Parser& templ,
			size_t minimum = 1,
			size_t maximum = 0) :
                _template(templ.copy()),
				_minimum(minimum),
				_maximum(maximum)
		{
		}

		virtual ~Repeat()
		{
			if (_item)
				delete _item;
		}
/*
        virtual bool read(
           bool bit
        ) override
        {
           throw std::logic_error("Should not reach here");
        }
*/
		virtual bool read(
           const Character& character
        ) override
		{

			if (_item == nullptr)
				_item = createItem();

			bool matched =
				_item->read(character);

			if (_item->_result == true)
			{
				matchedItem(_item);

				_item = createItem();

				++_matchedCount;

				if (_maximum > 0 &&
					_matchedCount > _maximum)
				{
					matched = false;
					setResult(false);
				}

				if (_matchedCount == _maximum)
					setResult(true);

			}
			else if (
				(_item->_result == false) ||
				(!matched))
			{
				if (_matchedCount >= _minimum)
				{
					setResult(true);
				}
				else
				{
					matched = false;
					setResult(false);
				}
			}
				
			return matched;
		}

		virtual void matchedItem(Parser *match)
		{
			delete match;
		}

		virtual Parser* createItem() {
			return _template->copy();
		}

        virtual Parser* copy() const {
           return new Repeat(*_template, _minimum, _maximum);
        }

	};

};

#endif