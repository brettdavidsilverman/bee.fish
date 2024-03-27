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

		virtual void setup(Parser* parser)
		{
			_parser = parser;

            if (!_match) {
	           _match = new T();
            }
		}
        
        

	};

}

#endif