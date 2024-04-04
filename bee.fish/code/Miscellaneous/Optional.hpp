#ifndef BEE_FISH_MISC__OPTIONAL_HPP
#define BEE_FISH_MISC__OPTIONAL_HPP

#include <iostream>
#if (__cplusplus >= 201703L || 1)
#include <optional>
#else
namespace std {

    struct nullopt_t {
    };
    
    const nullopt_t nullopt;

    template<class T>
    class optional {
    private:
        T _value;
        bool _hasValue;

    public:

        optional() : _value() {
            _hasValue = false;
        }

        optional(const T& value) : _value(value) {
            _hasValue = true;
        }

        optional(nullopt_t nullopt) : _value() {
            _hasValue = false;
        }

        bool hasValue() const {
            return _hasValue;
        }

        T& value() {
            if (!_hasValue)
                throw std::runtime_error("No value");
            return _value;
        }

        const T& value() const {
            if (!_hasValue)
                throw std::runtime_error("No value");
            return _value;
        }

        bool operator == (const T& value) const {
            if (_hasValue)
               return (_value == value);
            return false;
        }

        bool operator == (const optional<T>& value) const {
            if (_hasValue && value._hasValue)
               return _value == value._value;

            return (_hasValue == value._hasValue);
        }

        bool operator == (nullopt_t nullopt) {
            return !_hasValue;
        }

        bool operator != (nullopt_t nullopt) {
            return _hasValue;
        }

        optional<T>& operator = (nullopt_t nullopt) {
            _hasValue = false;
            return *this;
        }

        optional<T>& operator = (const T& value) {
            _value = value;
            _hasValue = true;
            return *this;
        }

        optional<T>& operator = (const optional<T>& value) {
            _value = value._value;
            _hasValue = value._hasValue;
            return *this;
        }

        operator bool () const {
            if (!_hasValue)
                return false;
            return (bool)_value;
        }

        inline friend std::ostream& operator << (std::ostream& out, const optional<bool>& value) {
            if (value == true)
               out << "true";
            else if (value == false)
               out << "false";
            else
               out << "null";
            return out;
        }

        
   };

}
#endif

namespace std {
    
#warning Buggy optional<bool>

   std::ostream& operator << (
      std::ostream& out,
      const std::optional<bool>& value
   )
   {
      if (value == std::nullopt)
         out << "null";
      else if (value == true)
         out << "true";
      else if (value == false)
         out << "false";
      else
         throw std::logic_error("Optional value");
      return out;
   }
  
   bool operator == (const optional<bool>& left, const optional<bool>& right)
   {
      // both have values
      if (left.has_value() &&
          right.has_value())
         return left.value() == right.value();
            
      // neither have values
      if (!left.has_value() &&
          !right.has_value())
         return true;
            
      // either or
      return false;
   }
   
   bool operator == (const optional<bool>& left, bool right)
   {
      // both have values
      if (left.has_value())
         return left.value() == right;
       
      return false;
            
   }
   
   bool operator == (const optional<bool>& left, nullopt_t right)
   {
      if (left.has_value())
         return false;
 
      return true;
   }
   
   bool operator != (const optional<bool>& left, const optional<bool>& right)
   {
      // both have values
      if (left.has_value() &&
          right.has_value())
         return left.value() != right.value();
            
      // neither have values
      if (!left.has_value() &&
          !right.has_value())
         return false;
            
      // either or
      return true;
   }
   
   bool operator != (const optional<bool>& left, bool right)
   {
      // both have values
      if (left.has_value())
         return left.value() != right;
           
      // either or
      return true;
   }
   
   bool operator != (const optional<bool> left, nullopt_t right)
   {
      // both have values
      if (left.has_value())
         return true;
           
      return false;
   }
   
   
}
      

#endif