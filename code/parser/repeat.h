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
        
        virtual bool isOptional() const 
        override
        {
            
            if (_minimum == 0)
                return true;
                
            return false;
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

				if (_maximum > 0)
                {
                   if (_matchedCount == _maximum)
                   {
				      success();
                   }
                   else if (_matchedCount > _maximum)
				   {
					  fail();
				   }
                }
              
                _item = createItem();
                

			}
			else if (_item->result() == false)
			{
                matched = false;
				if (_matchedCount >= _minimum)
				{
					success();
				}
				else
				{
					fail();
				}
			}
				
			return matched;
		}

		virtual void matchedItem(T *match)
		{
            ++_matchedCount;
			delete match;
		}
        
        virtual size_t matchedCount() const
		{
            return _matchedCount;
		}

		virtual T* createItem() {
			T* item = new T();
            
            if (_parser)
                item->setup(_parser);
               
            return item;
		}
        
        virtual void setup(Parser* parser) {
            
            if (_item)
                _item->setup(parser);
                
            Match::setup(parser);
            
        }
        
        virtual void eof(Parser* parser) {

           setup(parser);
           
           if (_item && _item->result() == nullopt)
           {
              _item->eof(parser);
              if (_item->result() == true)
                 ++_matchedCount;
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
       
	};

};

#endif
