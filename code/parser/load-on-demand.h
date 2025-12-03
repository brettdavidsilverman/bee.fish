#ifndef BEE_FISH_PARSER__LOAD_ON_DEMAMD_H
#define BEE_FISH_PARSER__LOAD_ON_DEMAMD_H

#include "match.h"

namespace BeeFishParser
{

	template<class T>
	class LoadOnDemand : public Match
	{

	public:
		LoadOnDemand(void* params = nullptr) : Match(params)
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
      
        {

            if (!_match) {

	           _match = new T();
               _match->setup(_parser);
               
            }
            
            
            return _match->match(_parser, character);
        }

	};

}

#endif