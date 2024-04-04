#ifndef BEE_FISH_JSON__SET_H
#define BEE_FISH_JSON__SET_H
#include <ostream>
#include <vector>
#include "../misc/optional.h"
#include "../parser/parser.h"
#include "blank-space.h"

using namespace std;

namespace BeeFishJSON
{

template<class OpenBrace, class Item, class Seperator, class CloseBrace>
class Set : public Match
{

protected:
   Item* _item;
   
   class _Seperator : public And {
   public:
      _Seperator() : And(
            new Optional(
               new BlankSpace()
            ),
            new Seperator(),
            new Optional(
               new BlankSpace()
            )
         )
      {

      }
   };

   class SubsequentItem : public Match {
   protected:
      Set* _set;
      Item* _item;
   public:

      SubsequentItem() : Match() {
         throw std::runtime_error("SubsequentItem construted without a set");
      }

      SubsequentItem(Match* set) : Match()
         
      {
         _set = dynamic_cast<Set*>(set);
         assert(_set);
         cerr << "SubsequentItem::SubsequentItem()" << endl;
      }
      
      virtual void setup(Parser* parser)
      {
         if (_parser)
            return;
            
         _parser = parser;
         
         _match = new And(
            new _Seperator(),
            new Invoke(
               _item = _set->createItem(),
               [this](Match* match)
               {
                  _set->onsetvalue(_item);
               }
            )
         );
         
         _match->setup(_parser);
      }

   };

   class SubsequentItems : public Repeat<SubsequentItem> {
   protected:
      Set* _set;
   public:
      SubsequentItems() : Repeat<SubsequentItem>(0) {
         throw std::runtime_error("SubsequentItems constructed without set");
      }

      SubsequentItems(Set* set) : Repeat<SubsequentItem>(0)
      {
         _set = set;
      }

      virtual ~SubsequentItems() {
      }

      virtual SubsequentItem* createItem() {
         SubsequentItem* item =
            new SubsequentItem(_set);
         
         if (Match::_parser)
            item->setup(Match::_parser);
            
         return item;
      }
   };


public:
   Set() : Match()
   {
   }

   virtual ~Set()
   {
   }

   virtual void setup(Parser* parser)
   {
      if (_parser)
         return;

      _parser = parser;
      
      _match =
         new And(
         new Invoke(
            new OpenBrace(),
            [this](Match* match) {
               this->onbeginset(match);
            }
         ),
         new Optional(
            new And(
               new Invoke(
                  _item = createItem(),
                  [this](Match* match) {
                     this->onsetvalue(_item);
                  }
               ),
               new Optional(
                  new SubsequentItems(this)
               )
            )
         ),
         new Invoke(
            new CloseBrace(),
            [this](Match* match) {
               this->onendset(match);
            }
         )
      );
      
      _match->setup(parser);

   }
   
   virtual Item* createItem()
   {
      Item* item = new Item(this);
      
      if (_parser)
         item->setup(_parser);
         
      return item;
   }
         
   virtual void onbeginset(Match* match)
   {
   }
   
   virtual void onsetvalue(Item* item)
   {
   }

   virtual void onendset(Match* match)
   {
   }


};


}

#endif

