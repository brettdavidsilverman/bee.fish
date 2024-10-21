#ifndef BEE_FISH_JSON__KEYED_SET_H
#define BEE_FISH_JSON__KEYED_SET_H
#include <ostream>
#include <vector>
#include "../Miscellaneous/Optional.hpp"
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
         Key* _key;
         KeyValueSeperator* _seperator;
         Value* _value;
      
         Invoke* _invokeKey;
         Invoke* _invokeSeperator;
         Invoke* _invokeValue;
         
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

      virtual _KeyValue* createItem() 
      override
      {
         _KeyValue* keyValue =
            new _KeyValue(this);
            
         if (Match::_parser)
            keyValue->setup(Match::_parser);
            
         return keyValue;
      }

      virtual void onsetupkeyvalue(Key* key, Value* value) {
         
      }
    
      virtual void onkeyvalue(Key* key, Value* value) {
      }
      
      virtual void onobjectkey(Key* key)
      {
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
         
      _invokeKey = new Invoke(
         _key = new Key(),
         [this](Match* match) {
            _set->onsetupkeyvalue(_key, _value);
            _set->onobjectkey(_key);
            return true;
         }
      );
         
      _seperator =
         new KeyValueSeperator();
         
      _invokeSeperator = new Invoke(
         _seperator,
         [this](Match* match)
         {
             _set->onsetupkeyvalue(_key, _value);
             return true;
         }
      );
      
      _invokeValue = new Invoke(
         _value = new Value(),
         [this](Match* match) {
            _set->onkeyvalue(_key, _value);
            return true;
         }
      );
         
      _match = new And(
          _invokeKey,
          _invokeSeperator,
          _invokeValue
      );
         
      _match->setup(parser);
         
   }
 
}

#endif