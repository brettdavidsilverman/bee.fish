#ifndef BEE_FISH_PARSER__JSON_H
#define BEE_FISH_PARSER__JSON_H

#include <map>
#include <iomanip>

namespace bee::fish::parser::json
{
   class JSON;
   class Object;
   class Number;
   class String;
   class Array;
   class BlankSpace;
}

#include "../parser.h"
#include "blank-space.h"
#include "value.h"
#include "number.h"
#include "string.h"
#include "array.h"
#include "object.h"

using namespace bee::fish::parser;

namespace bee::fish::parser::json
{
   
   class JSON : public And
   {
   public:
      JSON() : And(
         new Optional(
            new BlankSpace()
         ),
         new Or(
            new True(),
            new False(),
            new Null(),
            new Number(),
            new String(),
            new Array(),
            new Object()
         ),
         new Optional(
            new BlankSpace()
         )
      )
      
      {
         _inputs[1]->_capture = true;
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
            out << item();
         else
            And::write(out);
      }
      
      Match& item()
      {
         Or& _or = (Or&)(*this)[1];
         
         return _or.item();
      }

      virtual string name()
      {
         return "JSON";
      }
      
      virtual string& value()
      {
         return item().value();
      }
      
      virtual wstring& wvalue()
      {
         return item().wvalue();
      }
      
      virtual size_t index()
      {
         Or* _or = (Or*)_inputs[1];
         return _or->index();
      }
      
      virtual bool isNull()
      {
         return (index() == 2);
      }
   };
   
   
}

#endif