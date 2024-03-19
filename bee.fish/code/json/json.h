#ifndef BEE_FISH__JSON_H
#define BEE_FISH__JSON_H

#include "../parser/parser.h"
#include "version.h"
#include "blank-space.h"
#include "undefined.h"
#include "null.h"
#include "boolean.h"
#include "number.h"
#include "set.h"
#include "keyed-set.h"
#include "array.h"
#include "string.h"
#include "object.h"

//#include "../b-script/object.h"

#include "../power-encoding/power-encoding.h"


using namespace BeeFishParser;
using namespace BeeFishPowerEncoding;

namespace BeeFishJSON
{

   class JSONParser;
   
   enum Type {
      UNDEFINED,
      __NULL,
      BOOLEAN,
      NUMBER,
      STRING,
      ARRAY,
      OBJECT
   };

   class JSON : public And
   {
   public:
      Undefined* _undefined;
      Null*      _null;
      Boolean*  _boolean;
      Number*   _number;
      String*   _string;
      Array*     _array;
      Object*   _object;
      Or*        _items;

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
         
         _number  = new Number();
      
         _string  = new String();

         _array   = new Array();

         _object  = new Object();

         _items = new Or{
            _undefined,
            _null,
            _boolean,
            _number,
            _string,
            _array,
            _object
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
      
      virtual BString value()
      {
         return item().value();
      }

      virtual BString value() const
      {
         return item().value();
      }

      virtual bool matched() const {
         return _items->matched();
      }

      Type type() const {
         return (Type)(_items->_index);
      }

      JSONParser* jsonParser() {
         return (JSONParser*)_parser;
      }
   };

   
}

#endif