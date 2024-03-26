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

   public:
      Set() : Match()
      {
      }
            
      virtual ~Set()
      {
      }

      virtual Item* createItem() {
         return new Item();
      }  

      virtual void setup(Parser* parser)
      {
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

         class InvokeItem : public Match {
         protected:
            Set* _set;
         public:
            InvokeItem(Set* set) : Match(
               
            )
            {
               _set = set;
               _match = _set->createItem();
            }

            virtual void success() {
               Item* item = (Item*)_match;

               _set->matchedSetItem(item);
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

            SubsequentItem(Set* set) : Match(
            ),
               _set(set)
            {
            }
            
            virtual void setup(Parser* parser)
            override
            {
               _parser = parser;

			   if (!_match) {
                  _match = new And(
                     new _Seperator(), 
                     _item = _set->createItem()
                  );
                  _match->setup(parser);
               }
			
			   _setup = true;

			   Match::setup(parser);
            }
         
            virtual void success() {
cerr << "Set::subsequentItem " << _item->value() << endl;
               _set->matchedSetItem(_item);
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
               return new SubsequentItem(_set);
            }
         };

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
                     new InvokeItem(this),
                     new SubsequentItems(this)
                  )
               ),
               new Invoke(
                  new CloseBrace(),
                  [this](Match* match) {
                     this->onendset(match);
                  }
               )
            );

         Match::setup(parser);
      }
      
             
      virtual void matchedSetItem(Item* item)
      {
      }
      
      virtual BString& value()
      {
         return _match->value();
      }

      virtual void onbeginset(Match* match) {
      }
      
      virtual void onendset(Match* match) {
      }
      
      
   };
         

}

#endif

