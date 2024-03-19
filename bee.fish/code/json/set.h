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
               set->createItem()
            )
            {
               _set = set;
            }

            virtual void success() {
               _set->matchedSetItem((Item*)_match);
            }
         };

         class SubsequentItem : public And {
         public:
            
            SubsequentItem() : And() {
               throw std::runtime_error("SubsequentItem construted without a set");
            }

            SubsequentItem(Set* set) : And(
               new _Seperator(), 
               new InvokeItem(set)
            )
            {

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
      
      virtual BString value()
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

