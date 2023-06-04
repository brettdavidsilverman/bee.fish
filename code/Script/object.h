#ifndef BEE_FISH_B_SCRIPT__OBJECT_H
#define BEE_FISH_B_SCRIPT__OBJECT_H

#include <map>
#include <vector>
#include <memory>

#include "../b-string/string.h"
#include "../misc/constants.h"
#include "../json/json.h"

namespace BeeFishBScript {
   
   using namespace BeeFishBString;
   using namespace std;

   class Object;
   class Variable;

   typedef nullptr_t Null;
   typedef bool Boolean; 
   typedef double Number;
   typedef BString String;
   typedef vector<Variable> Array;

   typedef std::map<BString, Variable>                Map;
   typedef std::vector<BString>                       Table;
   typedef std::initializer_list<Map::value_type>     List;
   typedef std::initializer_list<Array::value_type>   ArrayList;

   typedef std::shared_ptr<BeeFishBScript::Object> ObjectPointer;
   typedef std::shared_ptr<BeeFishBScript::Array> ArrayPointer;

   #define undefined BeeFishBScript::Variable::Undefined()

   class Object : public Map {
   protected:
      Table _table;
      typedef Table::const_iterator const_iterator;
   public:

      Object() {

      }

      Object(List list)
      {
         loadMap(list);
      }

      Object(const Object& source) : Map(source), _table(source._table) {
      }

      virtual ~Object() {

      }

      virtual void clear() {
         Map::clear();
         _table.clear();
      }

      virtual void apply(const ObjectPointer value);

      virtual pair<Object::iterator,bool> insert(const Object::value_type& pair);

      Variable& operator[] (const BString& key) {

         if (count(key) == 0)
            _table.push_back(key);

         return Map::operator[](key);
         
      }

      Variable& operator[] (size_t index) {

         const BString& key = _table[index];

         return Map::operator[](key);
         
      }

      bool contains (const BString& key) const {

         return (count(key) > 0);
         
      }

      bool contains (const char* key) const {

         BString bstrKey(key);
         return contains(bstrKey);
         
      }

      //virtual Variable operator[] (const BString& key) const;
      //virtual Variable& operator[] (const BString& key);
      
      void loadMap(List list);

      virtual void write(ostream& out, size_t tabs = 0) const;

      friend ostream& operator << (ostream& out, const Object& object) {
         object.write(out);
         return out;
      }

      friend BStream& operator << (BStream& out, const Object& object) {
         object.write(out);
         return out;
      }

      friend BStream& operator << (BStream& out, const Variable& variable);

      virtual std::string str() const {
         stringstream stream;
         stream << *this;
         return stream.str();
      }

      virtual BString bstr() const {
         stringstream stream;
         stream << *this;
         return stream.str();
      }

      virtual size_t contentLength() const {
         BeeFishBString::BStream stream;
         stream << *this;
         stream.flush();
         return stream.totalSize();
      }

      const_iterator cbegin() const {
         return _table.cbegin();
      }

      const_iterator cend() const {
         return _table.cend();
      }

   };
   

   struct Variable {
   public:
      BeeFishJSON::Type _type;

      union Value {
         Boolean _boolean;
         Number _number;
         String _string;
         ArrayPointer _array;
         ObjectPointer _object;

         Value() {
         }

         Value(BeeFishJSON::Type type, const Value& source) {
            switch (type) {
            case BeeFishJSON::Type::UNDEFINED:
            case BeeFishJSON::Type::__NULL:
               break;
            case BeeFishJSON::Type::BOOLEAN:
               _boolean = source._boolean;
               break;
            case BeeFishJSON::Type::NUMBER:
               _number = source._number;
               break;
            case BeeFishJSON::Type::STRING:
               new (&_string) BString(source._string);
               break;
            case BeeFishJSON::Type::ARRAY:
               new (&_array) ArrayPointer(source._array);
               break;
            case BeeFishJSON::Type::OBJECT:
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
         _type(BeeFishJSON::Type::UNDEFINED)
      {

      } 

      Variable(const Variable& source) : 
         _type(source._type),
         _value(source._type, source._value)
      {
      }

      Variable(const Null& _nullptr) {
         _type = BeeFishJSON::Type::__NULL;
      }

      Variable(const Boolean& boolean) {
         _type = BeeFishJSON::Type::BOOLEAN;
         _value._boolean = boolean;
      }

      Variable(const Number& number) {
         _type = BeeFishJSON::Type::NUMBER;
         _value._number = number;
      }

      Variable(const int& number) : Variable(Number(number)) {
      }

      Variable(const unsigned int& number) : Variable(Number(number)) {
      }

      Variable(const void *& pointer) : Variable(Number((unsigned long)pointer)) {
      }

      Variable(const String& value) {
         _type = BeeFishJSON::Type::STRING;
         new (&_value._string) String(value);
      }

      Variable(const char* value) : Variable(BString(value)) {
      }

      Variable(const std::string& value) : Variable(BString(value)) {
      }

      Variable(const Array& value) {
         _type = BeeFishJSON::Type::ARRAY;
         new (&_value._array) ArrayPointer(new Array(value));
      }

      Variable(ArrayList list) :
         Variable(Array(list))
      {
      }

      Variable(ArrayPointer value) {
         _type = BeeFishJSON::Type::ARRAY;
         new (&_value._array) ArrayPointer(value);
      }

      Variable(const Object& value) {
         _type = BeeFishJSON::Type::OBJECT;
         new (&_value._object) ObjectPointer(new Object(value));
      }

      Variable(ObjectPointer value) {
         _type = BeeFishJSON::Type::OBJECT;
         new (&_value._object) ObjectPointer(value);
      }

      Variable(List list) :
         Variable(ObjectPointer(new Object(list)))
      {
      }

      static Variable& Undefined() {
         static Variable _undefined;
         return _undefined;
      }

      virtual ~Variable() {
         switch (_type) {
         case BeeFishJSON::Type::UNDEFINED:
         case BeeFishJSON::Type::__NULL:
         case BeeFishJSON::Type::BOOLEAN:
         case BeeFishJSON::Type::NUMBER:
            break;
         case BeeFishJSON::Type::STRING:
            _value._string.~String();
            break;
         case BeeFishJSON::Type::ARRAY:
            _value._array.~ArrayPointer();
            break;
         case BeeFishJSON::Type::OBJECT:
            _value._object.~ObjectPointer();
            break;
         }

      }

      virtual Variable& operator[] (const String& key) {
         ObjectPointer object = (ObjectPointer)*this;
         return (*object)[key];
      }

      virtual Variable& operator[] (const char* key) {
         ObjectPointer object = (ObjectPointer)*this;
         return (*object)[BString(key)];
      }

      virtual Variable& operator[] (size_t index) {
         ObjectPointer object = (ObjectPointer)*this;
         return (*object)[index];
      }

      virtual Variable& operator = (const Variable& source) {

         this->~Variable();
         _type = source._type;
         new (&_value) Value(source._type, source._value);
         return *this;
      }

      virtual bool operator == (const Variable& compare) const {

         if (_type == BeeFishJSON::Type::UNDEFINED && compare._type == BeeFishJSON::Type::UNDEFINED)
            return true;

         if (_type == compare._type) {
            return (memcmp(&_value, &compare._value, sizeof(Value)) == 0);
         }

         return false;

      }

      virtual bool operator != (const Variable& compare) const {

         if (_type == BeeFishJSON::Type::UNDEFINED && compare._type == BeeFishJSON::Type::UNDEFINED)
            return false;

         if (_type == compare._type) {
            return (memcmp(&_value, &compare._value, sizeof(Value)) != 0);
         }

         return true;

      }

      virtual bool operator == (const Null compare) const {

         if (_type == BeeFishJSON::Type::__NULL)
            return true;

         return false;

      }

      virtual bool operator != (const Null compare) const {

         if (_type != BeeFishJSON::Type::__NULL)
            return true;

         return false;

      }

      virtual bool operator == (bool compare) const {

         return (_type == BeeFishJSON::Type::BOOLEAN) && (_value._boolean == compare);

      }

      virtual bool operator == (double compare) const {

         return (_type == BeeFishJSON::Type::NUMBER) && (_value._number == compare);

      }

      virtual bool operator == (const char* compare) const {

         return (_type == BeeFishJSON::Type::STRING) && (_value._string == BString(compare));

      }

      friend ostream& operator << (ostream& out, const Variable& variable);

      virtual void write(ostream& out, size_t tabIndex = 0) const {
         switch (_type) {
         case BeeFishJSON::Type::UNDEFINED:
            out << "undefined";
            break;
         case BeeFishJSON::Type::__NULL:
            out << "null";
            break;
         case BeeFishJSON::Type::BOOLEAN:
            if (_value._boolean)
               out << "true";
            else
               out << "false";
            break;
         case BeeFishJSON::Type::NUMBER:
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
         case BeeFishJSON::Type::STRING:
            out << "\"";
            _value._string.writeEscaped(out);
            out << "\"";
            break;
         case BeeFishJSON::Type::ARRAY:
         {

            out << "[";            
            Array* array = _value._array.get();

            for  (Array::const_iterator it = array->cbegin();
                   it != array->cend();)
            {
               const Variable& element = *it;
               element.write(out, tabIndex + 1);
               if (++it != array->cend())
                  out << ", ";
            }

            out << "]";
            break;
         }
         case BeeFishJSON::Type::OBJECT: {
            _value._object->write(out, tabIndex + 1);
            break;
         }
         default:
            throw std::logic_error("JSON::Variable::write");
            return;
         }
      }

      virtual BString bstr() const {
         std::stringstream stream;
         stream << *this;
         return stream.str();
      }

      virtual std::string str() const {
         std::stringstream stream;
         stream << *this;
         return stream.str();
      }

      BeeFishBString::BString type() const {

         switch (_type) {
         case BeeFishJSON::Type::UNDEFINED:
            return "undefined";
         case BeeFishJSON::Type::__NULL:
            return "null";
         case BeeFishJSON::Type::BOOLEAN:
            return "Boolean";
         case BeeFishJSON::Type::NUMBER:
            return "Number";
         case BeeFishJSON::Type::STRING:
            return "String";
         case BeeFishJSON::Type::ARRAY:
            return "Array";
         case BeeFishJSON::Type::OBJECT:
            return "Object";
         default:
            throw std::logic_error("Invalid variable type");
         }

      }

      virtual size_t contentLength() const {
         BeeFishBString::BStream stream;
         stream << *this;
         stream.flush();
         return stream.totalSize();
      }
      

#define CHECK_TYPE(type) {if (_type != type) throw std::runtime_error("Cannot cast variable to type");}

      operator Boolean& (){
         CHECK_TYPE(BeeFishJSON::Type::BOOLEAN);
         return _value._boolean;
      }

      operator Number& () {
         CHECK_TYPE(BeeFishJSON::Type::NUMBER);
         return _value._number;
      }

      operator int () {
         CHECK_TYPE(BeeFishJSON::Type::NUMBER);
         return (int)(_value._number);
      }

      operator String& () {
         CHECK_TYPE(BeeFishJSON::Type::STRING);
         return _value._string;
      } 

      operator ArrayPointer () {
         CHECK_TYPE(BeeFishJSON::Type::ARRAY);
         return _value._array;
      }

      operator ObjectPointer() {
         CHECK_TYPE(BeeFishJSON::Type::OBJECT);
         return _value._object;
      }

   };

   inline void Object::write(ostream& out, size_t tabs) const {

      ostream& output = out;
      output << "{" << endl;

      if (tabs == 0)
         tabs++;

      bool emptySet = (_table.size() == 0);

      for (Object::const_iterator it = cbegin(); it != cend();) {
         const BString& key = *it;
         const Variable& value = at(key);
         if (value._type != BeeFishJSON::UNDEFINED) {
            if (tabs > 0)
               output << std::string(tabs * TabSpaces, ' ');
            output << "\"";
            key.writeEscaped(output);
            output << "\": ";
            value.write(output, tabs);
         }

         while (++it != _table.cend()) {
            const BString& key = *it;
            const Variable& value = at(key);
            if (value._type != BeeFishJSON::UNDEFINED) {
               output << "," << endl;
               break;
            }
         }
      }

      if (!emptySet)
         output << endl;

      if (tabs > 0)
         --tabs;

      if (tabs > 0)
         output << std::string(tabs * TabSpaces, ' ');

      output << "}";

   }
   
   inline void Object::loadMap(List list) {
      for (List::iterator it = list.begin(); it != list.end(); ++it) {
         insert(*it);
      }
   }

   inline pair<Object::iterator,bool> Object::insert(const Object::value_type& pair) {
      _table.push_back(pair.first);
      return Map::insert(pair);
   }

   inline void Object::apply(const ObjectPointer value) {
      const Object& object = *value;
      for (auto it = object.cbegin(); it != object.cend(); ++it) {
         const BString& key = *it;
         (*this)[key] = object.at(key);
      }
   }

   inline BStream& operator << (BStream& out, const Variable& variable) {
      variable.write(out);
      return out;
   }

   inline ostream& operator << (ostream& out, const Variable& variable) {
      variable.write(out);
      return out;
   }


/*
   inline Variable Object::operator[] (const BString& key) const {

      if (count(key) == 0)
         return Variable::Undefined();

      return Map::at(key);
      
   }

   inline Variable& Object::operator[] (const BString& key) {

      if (count(key) == 0)
         return Variable::Undefined();

      return Map::at(key);
      
   }
*/

}

#endif