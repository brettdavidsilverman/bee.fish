#ifndef BEE_FISH_MISC__OPTIONAL_H
#define BEE_FISH_MISC__OPTIONAL_H
/*
namespace BeeFishMisc {

    struct BeeFishMisc::nullopt_t {
    } BeeFishMisc::nullopt;

    template<class T>
    class optional {
    private:
        T _value;
        bool _hasValue = false;

    public:

        optional() {

        }

        optional(const T& value) {
            _value = value;
            _hasValue = true;
        }

        optional(BeeFishMisc::nullopt_t BeeFishMisc::nullopt) {
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

        bool operator == (const T& value) const {
            if (_hasValue)
               return _value == value;
            return false;
        }

        bool operator == (const optional<T>& value) const {
            if (_hasValue && value._hasValue)
               return _value == value._value;

            return (_hasValue == value._hasValue);
        }

        bool operator == (BeeFishMisc::nullopt_t BeeFishMisc::nullopt) {
            return !_hasValue;
        }

        bool operator != (BeeFishMisc::nullopt_t BeeFishMisc::nullopt) {
            return _hasValue;
        }

        optional<T>& operator = (BeeFishMisc::nullopt_t BeeFishMisc::nullopt) {
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

        operator const T& () const {
            if (!_hasValue)
                throw std::runtime_error("No value");
            return _value;
        }
    };
    
}

*/
#endif