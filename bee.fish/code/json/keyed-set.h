#ifndef BEE_FISH_JSON__KEYED_SET_H
#define BEE_FISH_JSON__KEYED_SET_H
#include <ostream>
#include <vector>
#include "../misc/optional.h"
#include "../parser/parser.h"
#include "blank-space.h"
#include "set.h"

using namespace std;

namespace BeeFishJSON
{

   template<
      class OpenBrace,
      class Key,
      class KeyValueSeperator,
      class Value,
      class Seperator,
      class CloseBrace
   > class KeyedSet;
   
   //template<class Key, class KeyValueSeperator, class Value>
      template<class OpenBrace, class Key, class KeyValueSeperator, class Value, class Seperator, class CloseBrace>
      class KeyValue : public Match {
      protected:
        typedef KeyedSet<OpenBrace, Key, KeyValueSeperator, Value, Seperator, CloseBrace> _KeyedSet;
        

        _KeyedSet* _set;
      
      public:
         Match* _key;
         Match* _seperator;
         Match* _value;
      
      public:
         KeyValue(Match* set) : Match() {
            _set = (_KeyedSet*)set;
         }
      
         // Defined below
         virtual void setup(Parser* parser);
      

      };
      
   template<
      class OpenBrace,
      class Key,
      class KeyValueSeperator,
      class Value,
      class Seperator,
      class CloseBrace
   >
   class KeyedSet :
      
      public
         Set<
            OpenBrace, 
            KeyValue<OpenBrace, Key, KeyValueSeperator, Value, Seperator, CloseBrace>,
            Seperator,
            CloseBrace
         >

   {
   public:
      typedef KeyValue<OpenBrace, Key, KeyValueSeperator, Value, Seperator, CloseBrace> _KeyValue;
       
       
   public:
      KeyedSet() : 
         Set<
            OpenBrace, 
            _KeyValue,
            Seperator,
            CloseBrace
         >()
      {
      }

      virtual Match* createItem() 
      override
      {
         return new _KeyValue(this);
      }

      virtual void onkey(Match* key) {
      }
      
      virtual void onvalue(Value* value) {
          
      }

   };


   // Declared above in KeyValue
   
   template<
      class OpenBrace,
      class Key,
      class KeyValueSeperator,
      class Value,
      class Seperator,
      class CloseBrace
   >
   void KeyValue<
         OpenBrace,
         Key,
         KeyValueSeperator,
         Value,
         Seperator,
         CloseBrace
    >::setup(Parser* parser)
    {
      if (_parser)
         return;
            
      _parser = parser;
         
      _key = new Invoke(
         new Key(),
         [this](Match* key) {
            _set->onkey(key);
         }
      );
         
      _seperator =
         new KeyValueSeperator();
         
      _value = new Invoke(
         new Value(),
         [this](Match* value) {
            _set->onsetvalue(value);
         }
      );
         
      _match = new And(
          _key,
          _seperator,
          _value
      );
         
      _match->setup(parser);
         
   }
 
}

#endif