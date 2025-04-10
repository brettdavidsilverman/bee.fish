#ifndef BEE_FISH__SCRIPT__VARIABLE_HPP
#define BEE_FISH__SCRIPT__VARIABLE_HPP

#include <map>
#include <vector>
#include <memory>
#include <cmath>

#include "../b-string/string.h"
#include "../Database/File.hpp"
#include "../Script/Type.hpp"
#include "Config.hpp"

namespace BeeFishScript {

   using namespace BeeFishDatabase;   
   using namespace BeeFishScript;
   using namespace std;

   class Object;
   class Variable;

   typedef nullptr_t Null;
   typedef bool Boolean; 
   typedef Size Integer;
   typedef long double Number;
   typedef BString String;
   typedef vector<Variable> Array;

   typedef std::map<String, Variable>                ObjectBase;
   typedef std::vector<String>                       Table;
   typedef std::initializer_list<ObjectBase::value_type>     List;
   typedef std::initializer_list<Array::value_type>   ArrayList;

   typedef std::shared_ptr<BeeFishScript::Object> ObjectPointer;
   typedef std::shared_ptr<BeeFishScript::Array> ArrayPointer;

   class Object : public ObjectBase {
   public:

      Object() {

      }

      Object(List list)
      {
         loadObject(list);
      }

      Object(const Object& source) :
         ObjectBase(source)
      {
      }

      virtual ~Object() {

      }

      virtual void apply(const ObjectPointer value);

      bool contains (const String& key) const {
         return (count(key) > 0);
      }

      bool contains (const char* key) const {
         String strKey(key);
         return contains(strKey);
      }

      void loadObject(List list);

      virtual void write(ostream& out, bool blankSpace, size_t tabs = 0) const;

      friend ostream& operator << (ostream& out, const Object& object) {
         object.write(out, true);
         return out;
      }

      //friend ostream& operator << (ostream& out, const Variable& variable);

      virtual std::string str(bool blankSpace = true) const {
         stringstream stream;
         this->write(stream, blankSpace);
         return stream.str();
      }

      virtual size_t contentLength() const {
         std::stringstream stream;
         stream << *this;
         stream.flush();
         return stream.str().size();
      }


   };
   

   struct Variable {
   public:
      Type _type;

      union Value {
         Boolean _boolean;
         Integer _integer;
         Number _number;
         String _string;
         ArrayPointer _array;
         ObjectPointer _object;

         Value() {
         }

         Value(Type type, const Value& source) {
            switch (type) {
            case Type::UNDEFINED:
            case Type::NULL_:
               break;
            case Type::BOOLEAN:
               _boolean = source._boolean;
               break;
            case Type::INTEGER:
               _integer = source._integer;
               break;
            case Type::NUMBER:
               _number = source._number;
               break;
            case Type::STRING:
               new (&_string) String(source._string);
               break;
            case Type::ARRAY:
               new (&_array) ArrayPointer(source._array);
               break;
            case Type::OBJECT:
               new (&_object) ObjectPointer(source._object);
               break;
            default:
               throw std::logic_error("JSON::Variable::Value::copy constructor");
            }
         }

         ~Value() {
         }

      } _value;

      Variable() :
         _type(Type::UNDEFINED)
      {

      }

      Variable(Type type)
      {
         _type = type;
         switch (_type) {
         case Type::UNDEFINED:
         case Type::NULL_:
         case Type::BOOLEAN:
            break;
         case Type::INTEGER:
            _value._integer = 0;
            break;
         case Type::NUMBER:
            _value._number = 0;
            break;
         case Type::STRING:
            new (&_value._string) String();
            break;
         case Type::ARRAY:
            new (&_value._array) ArrayPointer(new Array());
            break;
         case Type::OBJECT:
            new (&_value._object) ObjectPointer(new Object());
            break;
         case Type::UNKNOWN:
            break;
         }
      } 

      Variable(const Variable& source) : 
         _type(source._type),
         _value(source._type, source._value)
      {
      }

      Variable(const Null& _nullptr) {
         _type = Type::NULL_;
      }

      Variable(const Boolean& boolean) {
         _type = Type::BOOLEAN;
         _value._boolean = boolean;
      }
      
      Variable(const Integer& integer) {
         _type = Type::INTEGER;
         _value._integer = integer;
      }
      
      Variable(const int& integer) {
         _type = Type::INTEGER;
         _value._integer = integer;
      }

      Variable(const Number& number) {
         _type = Type::NUMBER;
         _value._number = number;
      }

      Variable(const void *& pointer) : Variable(Number((unsigned long)pointer)) {
      }

      Variable(const String& value) {
         _type = Type::STRING;
         new (&_value._string) String(value);
      }

      Variable(const char* value) : Variable(String(value)) {
      }

      Variable(const Array& value) {
         _type = Type::ARRAY;
         new (&_value._array) ArrayPointer(new Array(value));
      }

      Variable(ArrayList list) :
         Variable(Array(list))
      {
      }

      Variable(ArrayPointer value) {
         _type = Type::ARRAY;
         new (&_value._array) ArrayPointer(value);
      }

      Variable(const Object& value) {
         _type = Type::OBJECT;
         new (&_value._object) ObjectPointer(new Object(value));
      }

      Variable(ObjectPointer value) {
         _type = Type::OBJECT;
         new (&_value._object) ObjectPointer(value);
      }

      Variable(List list) :
         Variable(ObjectPointer(new Object(list)))
      {
      }
/*
      istream& operator >> (istream& in) {
         Variable variable;
         auto object = BeeFishScript::Object();
         return in >> object;
      }
*/
      static Variable Undefined() {
         Variable _undefined;
         return _undefined;
      }

      virtual ~Variable() {
         switch (_type) {
         case Type::UNDEFINED:
         case Type::NULL_:
         case Type::BOOLEAN:
         case Type::INTEGER:
         case Type::NUMBER:
            break;
         case Type::STRING:
            _value._string.~String();
            break;
         case Type::ARRAY:
            _value._array.~ArrayPointer();
            break;
         case Type::OBJECT:
            _value._object.~ObjectPointer();
            break;
         case Type::UNKNOWN:
            break;
         }

      }

      virtual Variable& operator[] (const String& key) {
         ObjectPointer object = (ObjectPointer)*this;
         return (*object)[key];
      }

      virtual Variable& operator[] (const char* key) {
         ObjectPointer object = (ObjectPointer)*this;
         return (*object)[String(key)];
      }

      virtual Variable& operator[] (Size index) {
         ArrayPointer array = (ArrayPointer)*this;
         return (*array)[index];
      }

      virtual Variable& operator = (const Variable& source) {

         this->~Variable();
         _type = source._type;
         new (&_value) Value(source._type, source._value);
         return *this;
      }

      virtual bool operator == (const Variable& compare) const {

         if (_type == Type::UNDEFINED && compare._type == Type::UNDEFINED)
            return true;

         if (_type == compare._type) {
            return (memcmp(&_value, &compare._value, sizeof(Value)) == 0);
         }

         return false;

      }

      virtual bool operator != (const Variable& compare) const {

         if (_type == Type::UNDEFINED && compare._type == Type::UNDEFINED)
            return false;

         if (_type == compare._type) {
            return (memcmp(&_value, &compare._value, sizeof(Value)) != 0);
         }

         return true;

      }

      virtual bool operator == (const Null compare) const {

         if (_type == Type::NULL_)
            return true;

         return false;

      }

      virtual bool operator != (const Null compare) const {

         if (_type != Type::NULL_)
            return true;

         return false;

      }

      virtual bool operator == (bool compare) const {

         return (_type == Type::BOOLEAN) && (_value._boolean == compare);

      }

      virtual bool operator == (Integer compare) const {

         return (_type == Type::INTEGER) && (_value._integer == compare);

      }
      
      virtual bool operator == (double compare) const {

         return (_type == Type::NUMBER) && (_value._number == compare);

      }

      virtual bool operator == (const char* compare) const {

         return (_type == Type::STRING) && (_value._string == String(compare));

      }

      friend ostream& operator << (ostream& out, const Variable& variable) {
         variable.write(out, true);
         return out;
      }

      virtual void write(ostream& out, bool blankSpace, size_t tabIndex = 0) const {
         switch (_type) {
         case Type::UNDEFINED:
            out << "undefined";
            break;
         case Type::NULL_:
            out << "null";
            break;
         case Type::BOOLEAN:
            if (_value._boolean)
               out << "true";
            else
               out << "false";
            break;
         case Type::INTEGER:
            out << _value._integer;
            break;
         case Type::NUMBER:
            if ( _value._number == 0.0 ) {
               out << "0";
            } 
            else if (isnormal(_value._number)) {
//               out.precision(17);
               out << _value._number;
            }
            else {
               // JSON doesn't recognise NaN,
               // so make it a string
               out << "\"NaN\"";
            }
            break;
         case Type::STRING:
           // out << "\""_value._string;
          //  break;

            out << "\"";
            out << BeeFishMisc::escape(_value._string);
            out << "\"";
            break;

         case Type::ARRAY:
         {

            out << "[";            
            const Array* array = _value._array.get();
            if (array->size() > 1) {
               if (blankSpace)
                  out << "\r\n";
               tabIndex++;
            }
            
            
            
            for  (Array::const_iterator it = array->cbegin();
                   it != array->cend();)
            {
               if (array->size() > 1 && blankSpace)
                  out << std::string(tabIndex * TAB_SPACES, ' ');
               const Variable& element = *it;
               element.write(out, blankSpace, tabIndex);
               if (++it != array->cend() && blankSpace ) {
                  out << ",\r\n";
               }
            }

            
            
            if (array->size() > 1) {
                --tabIndex;
               if (blankSpace) {
                  out << "\r\n";
                  out << std::string(tabIndex * TAB_SPACES, ' ');
               }
            }
            out << "]";
            break;
         }
         case Type::OBJECT: {
            _value._object->write(out, blankSpace, tabIndex + 1);
            break;
         }
         default:
            throw std::logic_error("JSON::Variable::write");
            return;
         }
      }

      virtual String str(bool blankSpace = true) const {
         std::stringstream stream;
         this->write(stream, blankSpace);
         return stream.str();
      }

      String type() const {

         stringstream stream;
         stream << ToChar(_type);
         return stream.str();

      }

      virtual size_t contentLength() const {
         stringstream stream;
         stream << *this;
         return stream.str().size();
      }

      virtual Type getType() const {
         return _type;
      }

#define CHECK_TYPE(type) {if (_type != type) throw std::runtime_error("Cannot cast variable to type");}

      operator Boolean& (){
         CHECK_TYPE(Type::BOOLEAN);
         return _value._boolean;
      }
      
      operator Integer& () {
         CHECK_TYPE(Type::INTEGER);
         return _value._integer;
      }
      
      operator Number& () {
         CHECK_TYPE(Type::NUMBER);
         return _value._number;
      }

      operator String& () {
         CHECK_TYPE(Type::STRING);
         return _value._string;
      } 

      operator ArrayPointer () {
         CHECK_TYPE(Type::ARRAY);
         return _value._array;
      }

      operator ObjectPointer() {
         CHECK_TYPE(Type::OBJECT);
         return _value._object;
      }
   };

   inline void Object::write(ostream& out, bool blankSpace, size_t tabs) const {

      ostream& output = out;
      
      bool emptySet = (size() == 0);

      if (emptySet) {
         output << "{}";
         return;
      }

      if (tabs == 0)
         tabs++;

      output << "{";
      
      if (blankSpace)
         output << "\r\n";

      for (Object::const_iterator it = cbegin(); it != cend();) {
         const String& key = it->first;
         const Variable& value = it->second;
         if (value._type != Type::UNDEFINED) {
            if (tabs > 0 && blankSpace)
               output << std::string(tabs * TAB_SPACES, ' ');
            output << "\"";
            output << BeeFishMisc::escape(key);
            output << "\":";
            if (blankSpace)
               output << " ";
            value.write(output, blankSpace, tabs);
         }

         if (++it != cend()) {
            output << ",";
            if (blankSpace)
               output << "\r\n";
         }

      }

      if (blankSpace)
         output << "\r\n";

      if (tabs > 0)
         --tabs;

      if (tabs > 0 && blankSpace)
         output << std::string(tabs * TAB_SPACES, ' ');

      output << "}";

   }
   
   inline void Object::loadObject(List list) {
      for (List::iterator it = list.begin(); it != list.end(); ++it) {
         insert(*it);
      }
   }

   inline void Object::apply(const ObjectPointer value) {
      const Object& object = *value;
      for (auto it = object.cbegin(); it != object.cend(); ++it) {
         const String& key = it->first;
         (*this)[key] = it->second;
      }
   }

   const Variable null(Type::NULL_);
   const Variable undefined(Type::UNDEFINED);
}

#endif