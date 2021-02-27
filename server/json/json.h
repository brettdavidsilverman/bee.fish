#ifndef BEE_FISH__JSON_H
#define BEE_FISH__JSON_H

#include <map>
#include <iomanip>

namespace bee::fish::json
{
   class JSON;
   class Object;
   class Number;
   class String;
   class Array;
   class BlankSpace;
}

#include "../parser/parser.h"
#include "blank-space.h"
#include "number.h"
#include "string.h"
#include "array.h"
/*
#include "object.h"
*/
using namespace bee::fish::parser;

namespace bee::fish::json
{
   
   class _JSON : public And
   {
   public:
      Or* _switch;
      
   public:
      _JSON() : And(
         new Optional(
            new BlankSpace()
         ),
         _switch = new Or(
            new True(),
            new False(),
            new Null(),
            new Number(),
            new String(),
            new Array()
            //new Object()
         ),
         new Optional(
            new BlankSpace()
         )
      )
      
      {
      }
            
      class True : public Word
      {
      public:
         True() : Word("true")
         {
         }
         
         virtual string name()
         {
            return "true";
         }
      };
      
      class False : public Word
      {
      public:
         False(): Word("false")
         {
         }
         
         virtual string name()
         {
            return "false";
         }

      };
      
      class Null : public Word
      {
      public:
         Null(): Word("null")
         {
         }
         
         virtual string name()
         {
            return "null";
         }

      };
      
      virtual void write(ostream& out)
      { 
         if (result() == true)
            out << *item();
         else
            And::write(out);
      }
      
      Match* item()
      {
         return _switch->_item;
      }

      virtual size_t index()
      {
         return _switch->index();
      }
      
      virtual bool isNull()
      {
         return (index() == 2);
      }
   };
   
   Match* JSON = new _JSON();
}

#endif