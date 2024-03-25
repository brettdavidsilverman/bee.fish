#ifndef BEE_FISH_PARSER__LOAD_ON_DEMAMD_H
#define BEE_FISH_PARSER__LOAD_ON_DEMAMD_H

#include "match.h"

namespace BeeFishParser
{

	template<class T>
	class LoadOnDemand : public Match
	{

	public:
		LoadOnDemand() : Match()
		{
		}

		virtual ~LoadOnDemand() {
		}

		virtual void setup(Parser* parser)
		{
            if (_setup)
               return;
               
			_parser = parser;
            
            _match = new T();
			
			_setup = true;

			Match::setup(parser);
		}
        
        

	};

}

#endif