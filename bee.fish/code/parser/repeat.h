#ifndef BEE_FISH_PARSER__REPEAT_H
#define BEE_FISH_PARSER__REPEAT_H

#include <vector>
#include "match.h"
namespace BeeFishParser
{

	template<class T>
	class Repeat : public Match
	{
	private:
		T *_item;

	public:
		size_t _minimum = 1;
		size_t _maximum = 0;
		size_t _matchedCount = 0;

	public:
		Repeat(
			size_t minimum = 1,
			size_t maximum = 0) :
				_minimum(minimum),
				_maximum(maximum)
		{
			_item = nullptr;
		}

		virtual ~Repeat()
		{
			if (_item)
				delete _item;
		}

		virtual bool matchCharacter(const Char &character)
		{

			if (_item == nullptr)
				_item = createItem();

			bool matched =
				_item->match(_parser, character);

			if (_item->_result == true)
			{

				matchedItem(_item);

				_item = createItem();

				++_matchedCount;

				if (_maximum > 0 &&
					_matchedCount > _maximum)
				{
					matched = false;
					_result = false;
				}

				if (_matchedCount == _maximum)
					_result = true;

			}
			else if (
				(_item->_result == false) ||
				(!matched))
			{
				if (_matchedCount >= _minimum)
				{
					_result = true;
				}
				else
				{
					matched = false;
					_result = false;
				}
			}
				
			return matched;
		}

		virtual void matchedItem(T *match)
		{
			delete match;
		}

		virtual T* createItem() {
			return new T();
		}

	};

};

#endif
