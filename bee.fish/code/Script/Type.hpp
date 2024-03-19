#ifndef BEE_FISH__SCRIPT__TYPE_HPP
#define BEE_FISH__SCRIPT__TYPE_HPP

namespace BeeFishScript {

   enum class Type {
      UNDEFINED,
      NULL_,
      BOOLEAN,
      INTEGER,
      NUMBER,
      STRING,
      ARRAY,
      OBJECT
   };

   
   ostream& operator << (ostream& out, const Type& type)
   {
      switch (type)
      {
         case Type::UNDEFINED:
            out << "undefined";
            break;
         case Type::NULL_:
            out << "null";
            break;
         case Type::BOOLEAN:
            out << "bool";
            break;
         case Type::INTEGER:
            out << "integer";
            break;
         case Type::NUMBER:
            out << "number";
            break;
         case Type::STRING:
            out << "string";
            break;
         case Type::ARRAY:
            out << "array";
            break;
         case Type::OBJECT:
            out << "object";
            break;
         default:
            out << "UNKNOWN";
      }

      return out;
   }

   PowerEncoding& operator << (PowerEncoding& out, const Type& type)
   {
      out << (int)type;
      return out;
   }

   PowerEncoding& operator >> (PowerEncoding& in, Type& value)
   {
      int type;
      in >> type;
      value = (Type)type;
      return in;
   }


}

#endif