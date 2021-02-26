#ifndef BEE_FISH_PARSER__REPEAT_H
#define BEE_FISH_PARSER__REPEAT_H

#include "match.h"
namespace bee::fish::parser
{
   
   template<class T>
		class Repeat : public Match
		{
		private:
			  T* _match;
			   
		protected:
			  vector<Match*> _items;
			  size_t _matchedCount = 0;
			  
		public:
			
			  Repeat() : Match()
			  {
			     _match = NULL;
			  }
			   
			  virtual ~Repeat() {
			   
			     for (auto
			             it = _items.cbegin();
			             it != _items.cend();
			             ++it)
			     {
			        Match* child = *it;
			        if (child)
			        {
			           delete child;
			        }
			     }
			      
			     if (_match) {
			        delete _match;
			        _match = NULL;
			     }
			  }
			   
			   
			  virtual bool match(int character)
			  {
			
			     if (_match == NULL)
			        _match = createItem();
			         
			     bool matched =
			        _match->match(character);
			         
			     if (matched)
			        Match::match(character);
			      
			     if (_match->result() == true)
			     {
			      
			        addItem(_match);
			         
			        _match = createItem();
			         
			        ++_matchedCount;
			     }
			     else if (
			           (_match->result() == false) ||
			           (!matched) ||
			           (character == Match::EndOfFile)
			        )
			     {
			        if (_matchedCount > 0)
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
			   
			  virtual vector<Match*>& items() {
			      return _items;
			  }
			   
			  virtual T* createItem() {
			     return new T();
			  }
			   
			  virtual void addItem(Match* match) {
			     delete match;
			     return;
			     items().push_back(
			        match
			     );
			      
			  }
			   
			   
			  virtual string name()
			  {
			     return
			        "Repeat" + 
			        to_string(_matchedCount);
			  }
			   
			  Repeat(const Repeat& source) 
      {
         _match = NULL;
      }
			   
      virtual Match* copy() const
      {
         return new Repeat(*this);
      }
			
   };


};

#endif
