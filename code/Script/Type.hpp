#ifndef BEE_FISH__SCRIPT__TYPE_HPP
#define BEE_FISH__SCRIPT__TYPE_HPP

#include "../power-encoding/encoding.h"

namespace BeeFishScript {

   using namespace BeeFishPowerEncoding;
   
   enum class Type {
      UNDEFINED,
      NULL_,
      BOOLEAN,
      INTEGER,
      NUMBER,
      STRING,
      ARRAY,
      OBJECT,
      UNKNOWN
   };
   
   

   std::string toChar (const Type& type)
   {
      switch (type)
      {
         case Type::UNDEFINED:
            return "undefined";
            break;
         case Type::NULL_:
            return "null";
            break;
         case Type::BOOLEAN:
            return "bool";
            break;
         case Type::INTEGER:
            return "integer";
            break;
         case Type::NUMBER:
            return "number";
            break;
         case Type::STRING:
            return "string";
            break;
         case Type::OBJECT:
            return "object";
            break;
         case Type::ARRAY:
            return "array";
            break;
         case Type::UNKNOWN:
            return "UNKNOWN";
      }

      stringstream stream;
      stream << (int)type;
      
      return stream.str();
   }
   
   ostream& operator << (ostream& out, const Type& type)
   {
      out << toChar(type);
      return out;
   }

   PowerEncoding& operator << (PowerEncoding& out, const Type& type)
   {
      out << (unsigned int)type;
      return out;
   }

   PowerEncoding& operator >> (PowerEncoding& in, Type& value)
   {
      unsigned int type;
      in >> type;
      value = (Type)type;
      return in;
   }


}

#endif