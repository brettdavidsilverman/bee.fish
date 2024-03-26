#ifndef BEE_FISH__JSON_H
#define BEE_FISH__JSON_H

#include "../parser/parser.h"
#include "version.h"
#include "blank-space.h"
#include "undefined.h"
#include "null.h"
#include "boolean.h"
#include "integer.h"
#include "number.h"
#include "set.h"
#include "keyed-set.h"
#include "array.h"
#include "string.h"
#include "object.h"
#include "../Script/Type.hpp"

//#include "../b-script/object.h"

#include "../power-encoding/power-encoding.h"


using namespace BeeFishParser;
using namespace BeeFishPowerEncoding;
using namespace BeeFishScript;

namespace BeeFishJSON
{

   class JSONParser;

   class JSON : public And
   {
   public:
      Undefined* _undefined;
      Null*      _null;
      Boolean*  _boolean;
      //Integer*  _integer;
      Number*   _number;
      String*   _string;
      Array*     _array;
      Object*   _object;
      
      Or*        _items;
      Type       _type = Type::UNKNOWN;
      
   protected:
      Invoke* invoke(Match* match, Type type)
      {
         return new Invoke(
            match,
            [this, type](Match* match) {
               _type = type;
               onvalue(type, match->value());
            }
         );
      }
      
      Invoke* invokeNumber(Match* match)
      {
         return new Invoke(
            match,
            [this](Match* match) {
               BString& value = match->value();
               if (value.contains('-') ||
                   value.contains('+') ||
                   value.contains('e') ||
                   value.contains('E') ||
                   value.contains('.'))
                  _type = Type::NUMBER;
               else
                  _type = Type::INTEGER;
               onvalue(_type, value);
            }
         );
      }
      
   public:
      virtual void onvalue(Type type, BString& value)
      {
      }
      
   public:
      JSON() : And()
      {
      }
      
      virtual ~JSON()
      {

      }

      // Defined in json-parser.h
      virtual void success();
      
      virtual void setup(Parser* parser)
      {
         _undefined = new Undefined();

         _null    = new Null();
      
         _boolean = new Boolean();
         
        // _integer = new Integer();
         
         _number  = new Number();
  
         _string = new String();
         
         _array   = new Array();

         _object  = new Object();

         _items = new Or{
            invoke(_undefined, Type::UNDEFINED),
            invoke(_null, Type::NULL_),
            invoke(_boolean, Type::BOOLEAN),
            invokeNumber(_number),
            invoke(_string, Type::STRING),
            invoke(_array, Type::ARRAY),
            invoke(_object, Type::OBJECT)
         };
         
         _inputs = {
            new Optional(new BlankSpace()),
            _items,
         };
        
         And::setup(parser);

         
         
      }
      
      
      virtual Match& item() const
      {
         if (!_items->matched())
            throw runtime_error("No JSON item matched");

         return _items->item();
      }
      
      bool isNull() const
      {
         return _null->matched();
      }
      
      virtual BString& value()
      {
         return item().value();
      }

      virtual const BString& value() const
      {
         return item().value();
      }

      virtual bool matched() const {
         return _items->matched();
      }

      Type type() const {
         return _type;
      }

      JSONParser* jsonParser() {
         return (JSONParser*)_parser;
      }
      
      virtual void eof(Parser* parser)
      override {
 
          if (!_setup)
             setup(parser);
             
          assert(_parser);
          assert(_items);
          
          if (result() == nullopt &&
              type() == Type::UNKNOWN)
          {
             _number->eof(parser);
             //_integer->eof(parser);
          }
          

      }
   };

   
}

#endif