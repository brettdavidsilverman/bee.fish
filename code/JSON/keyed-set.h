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

   template<class Key, class Value>
   class KeyMatched {
   public:
      KeyMatched() {

      }

      virtual void matchedKey(Key& key, Value& value) {

      }
   };

   template<class Key, class KeyValueSeperator, class Value>
   class KeyValue : public And {
   public:
      KeyValue() : And() {
         throw std::logic_error("Invalid constructor");
      }
   public:
      KeyMatched<Key, Value>* _keyMatched;
      Key* _key;
      KeyValueSeperator* _seperator;
      Value* _value;

      class InvokeKeyValueSeperator : public KeyValueSeperator {
      protected:
         KeyMatched<Key, Value>* _keyMatched;
         Key* _key;
         Value* _value;
      public:
         InvokeKeyValueSeperator(KeyMatched<Key, Value>* keyMatched, Key* key, Value* value) 
            : KeyValueSeperator() 
         {
            _keyMatched = keyMatched;
            _key = key;
            _value = value;
         }

         virtual void success() {
            Key& key = *_key;
            Value& value = *_value;

            if (!value._setup)
                value.setup(this->_parser);

            _keyMatched->matchedKey(key, value);

            KeyValueSeperator::success();
         }

      };

   public:
      KeyValue(KeyMatched<Key, Value>* keyMatched) : And() {
         _keyMatched = keyMatched;
      }
      
      virtual void setup(Parser* parser) {
         _key = new Key();
         _value = new Value();
         _seperator = new InvokeKeyValueSeperator(_keyMatched, _key, _value);

         And::_inputs = {
            _key,
            new Optional(new BlankSpace()),
            _seperator,
            new Optional(new BlankSpace()),
            _value
         };

         And::setup(parser);
      }
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
            KeyValue<Key, KeyValueSeperator, Value>,
            Seperator,
            CloseBrace
         >,
         KeyMatched<Key, Value>

   {
   public:
      typedef KeyValue<Key, KeyValueSeperator, Value> _KeyValue;

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

      virtual _KeyValue* createItem() {
         return new _KeyValue(this);
      }

      virtual void matchedKey(Key& key, Value& value) {
      }

   };

}

#endif