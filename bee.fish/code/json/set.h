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
      Match* _item;
   public:

      SubsequentItem() : Match() {
         throw std::runtime_error("SubsequentItem construted without a set");
      }

      SubsequentItem(Match* set) : Match(),
         _set((Set*)set)
      {
         _match = new And(
            new _Seperator(),
            new Invoke(
               _item = _set->createItem(),
               [this](Match* match)
               {
                  _set->onsetvalue(match);
               }
            )
         );
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
                  createItem(),
                  [this](Match* match) {
                     this->onsetvalue(match);
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
   
   virtual Match* createItem()
   {
      return new Item(this);
   }
         
   virtual void onbeginset(Match* match) {
   }
   
   virtual void onsetvalue(Match* item)
   {
   }
   

   virtual void onendset(Match* match) {
   }


};


}

#endif

