#ifndef BEE_FISH_PARSER__LOAD_ON_DEMAMD_H
#define BEE_FISH_PARSER__LOAD_ON_DEMAMD_H

#include "match.h"

namespace BeeFishParser
{

	template<class T>
	class LoadOnDemand : public Match
	{

	public:
		LoadOnDemand(void* params = nullptr) :
            Match()
		{
            
		}

		virtual ~LoadOnDemand() {
		}
        
        virtual bool isLoadOnDemand() const 
        {
            return true;
        }

		virtual void setup(Parser* parser)
        override
		{
            if (_parser)
               return;
               
			_parser = parser;
            
            
		}

        virtual bool match(
            Parser* parser,
            const Char& character
        )
        override
        {

            if (!_match) {

	           _match = createItem();
               _match->setup(_parser);
               
            }
            
            
            return _match->match(_parser, character);
        }
        
        virtual T* createItem() {
            return new T();
        }
        
        T* item()
        {
            return (T*)_match;
        }
        
        const T* item() const
        {
            return (T*)_match;
        }

	};

}

#endif