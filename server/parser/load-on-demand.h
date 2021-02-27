#ifndef BEE_FISH_PARSER__LOAD_ON_DEMAND_H
#define BEE_FISH_PARSER__LOAD_ON_DEMAD_H

#include "match.h"

namespace bee::fish::parser {

		class LoadOnDemand : public Match
		{
		private:
		   const Match* _template;
		   Match* _item;
		
		public:
		   LoadOnDemand(const Match* template_) :
		      _template(template_)
		   {
		      _item = NULL;
		   }
		   
		   virtual bool match(int character)
		   {
		      if (!_item)
		         _item = createItem();
		      
		      bool matched =
		         _item->match(character);
		      
		      if (matched)
		         Match::match(character);
		      
		      if (_item->result() == true) {
		         success();
		      }
		      else if (_item->result() == false) {
		         fail();
		      }
		         
		      return matched;
		   }
		   
		   virtual Match* createItem()
		   {
		      return _template->copy();
		   }
		   
		   virtual ~LoadOnDemand() {
		      if (_item)
		         delete _item;
		   }

		   
		   LoadOnDemand(const LoadOnDemand& source) :
		      _template(source._template)
      {
         _item = NULL;
      }
			   
      virtual Match* copy() const
      {
         return new LoadOnDemand(*this);
      }
			   
   };

}

#endif