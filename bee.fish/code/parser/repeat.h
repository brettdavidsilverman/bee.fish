#ifndef BEE_FISH_PARSER__REPEAT_H
#define BEE_FISH_PARSER__REPEAT_H

#include <vector>
#include "match.h"
namespace BeeFishParser
{

	template<class T>
	class Repeat : public Match
	{
	protected:
		T* _item = nullptr;

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

			if (_item->result() == true)
			{

				matchedItem(_item);

				_item = createItem();

				++_matchedCount;

				if (_maximum > 0 &&
					_matchedCount > _maximum)
				{
					matched = false;
					fail();
				}

				if (_matchedCount == _maximum)
					success();

			}
			else if (
				(_item->_result == false) ||
				(!matched))
			{
				if (_matchedCount >= _minimum)
				{
					success();
				}
				else
				{
					matched = false;
					fail();
				}
			}
				
			return matched;
		}

		virtual void matchedItem(T *match)
		{
			delete match;
		}

		virtual T* createItem() {
			T* item = new T();
            
            if (_parser)
                item->setup(_parser);
               
            return item;
		}
        
        virtual void eof(Parser* parser) {

           setup(parser);
           
           if (result() == nullopt)
           {
              if (_matchedCount == 0)
              {
                 if (_item && _item->result() == nullopt)
                 {
                    _item->eof(parser);
                    if (_item->result() == true)
                       ++_matchedCount;
                 }
              }
              
              if (_matchedCount >= _minimum &&
                  (_matchedCount <= _maximum ||
                   _maximum == 0))
              {
                 success();
              }
              else
                 fail();
           }
        }
	};

};

#endif
