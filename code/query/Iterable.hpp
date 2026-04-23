#ifndef BEE_FISH__QUERY__ITERABLE_HPP
#define BEE_FISH__QUERY__ITERABLE_HPP

#include "../Database/Database.hpp"

namespace BeeFishQuery {

using namespace std;
using namespace BeeFishPowerEncoding;
using namespace BeeFishDatabase;

    class Iterable
    {
    protected:
        JSONDatabase& _database;
        PathBase& _path;
    public:

        Iterable(
            JSONDatabase& database,
            PathBase& path
        ) :
            _database(database),
            _path(path)
        {
        }
        
        virtual ~Iterable()
        {
        }
        
        class Iterator {
        protected:
            typedef PathBase::PathIterator<JSONPath::Id> _Iterator;
            _Iterator _iterator;
            JSONDatabase* _database = nullptr;
            BString _parentValue;
            BString _value;
            JSONPath _parentPath;
            JSONPath _jsonPath;
        public:
            // Iterator traits (required for STL compatibility in C++17 and earlier)
            using iterator_category = std::forward_iterator_tag;
            using value_type        = BString;
            using difference_type   = std::ptrdiff_t;
            using pointer           = const BString*;
            using reference         = const BString&;

            Iterator()
            {
            }
            
            Iterator(
                Iterable* iterable
            ) :
                _iterator(&iterable->_path),
                _database(&iterable->_database)
            {
                setValue();
            }
            
            void setValue()
            {
                
                

                while (!_iterator._isEnd) {
                    _parentValue = 
                        toString(_iterator);
                    _parentPath =
                        jsonPath(_iterator);
                        
                    save();
                    _Iterator iterator = _iterator;
                    ++iterator;
                    if (!iterator._isEnd) {
                        _value = toString(iterator);
                        _jsonPath = jsonPath(iterator);
                        if (_value.startsWith(_parentValue))
                        {
                            _parentValue = _value;
                            _parentPath = _jsonPath;
                            _iterator = iterator;
                        }
                        else {
                            _value = _parentValue;
                            _jsonPath = _parentPath;
                            restore();
                            break;
                        }
                    }
                    else {
                        _value = _parentValue;
                        _jsonPath = _parentPath;
                        restore();
                        break;
                    }
                }
                
                
            }
            
            BString toString(_Iterator& iterator) 
            {
                
                return jsonPath(iterator).toString();

            }
            
            JSONPath jsonPath(_Iterator& iterator)
            {
                Path path = _database->objects()[*iterator];
                assert(path.hasData());
                Index index = path.getData<Index>();
                
                JSONPath json(
                    *_database,
                    index
                );
                
                return json;
                
            }
            
            JSONPath jsonPath()
            {
                return _jsonPath;
            }
        
            // Dereference operator (*)
            reference operator*() const
            {
                return _value;
            }
            
            pointer operator->() const
            {
                return &_value;
            }

            // Prefix increment operator (++)
            Iterator& operator++() {
                assert(!_iterator._isEnd);
                ++_iterator;
                setValue();
                return *this;
            }

            // Postfix increment operator (++)
            Iterator operator++(int) 
            {
                assert(!_iterator._isEnd);
                Iterator tmp = *this;
                
                ++(*this);
                return tmp;
            }
            
            void save()
            {
                _iterator.save();
            }
            
            void restore()
            {
                _iterator.restore();
            }

            friend bool operator == (
                const Iterator& a,
                const Iterator& b
            )
            {
                return  (a._iterator == b._iterator);
            }
            
            friend bool operator != (
                const Iterator& a,
                const Iterator& b
            )
            { 
                return (a._iterator != b._iterator);
            }

        
        };
        
        
        class ReversePathIterator {
        protected:
            PathBase::ReversePathIterator<JSONPath::Id> _iterator;
            
            BString _value;
            
        public:
            // Iterator traits (required for STL compatibility in C++17 and earlier)
            using iterator_category = std::forward_iterator_tag;
            using value_type        = BString;
            using difference_type   = std::ptrdiff_t;
            using pointer           = const BString*;
            using reference         = const BString&;

            // Constructor
            ReversePathIterator() {
            }
            
            ReversePathIterator(Iterable* iterable) :
                _iterator(&iterable->_path)
            {
            
            }
            
            // Dereference operator (*)
            reference operator*() const
            {
                return _value;
            }

            // Arrow operator (->)
            pointer operator->() const 
            {
                return &_value; 
            }

            // Prefix increment operator (++)
            ReversePathIterator& operator++() 
            {
                assert(!_iterator._isEnd);
                ++_iterator;
                return *this;
            }

            // Postfix increment operator (++)
            ReversePathIterator operator++(int) {
                ReversePathIterator tmp = *this;
                ++(*this);
                return tmp;
            }

            friend bool operator == (
                const ReversePathIterator& a,
                const ReversePathIterator& b
            )
            {
                return  (a._iterator == b._iterator);
            }
            
            friend bool operator != (
                const ReversePathIterator& a,
                const ReversePathIterator& b
            )
            { 
                return (a._iterator != b._iterator);
            }

        
        };
   
        virtual Iterator begin() 
        {
            return Iterator(this);
        }
    
        // Points one past the last element
        Iterator end()
        { 
            Iterator iterator;
            return iterator;
        }
        
        virtual ReversePathIterator rbegin() 
        {
            return ReversePathIterator(this);
        }
    
        // Points one past the last element
        ReversePathIterator rend()
        { 
            ReversePathIterator iterator;
            return iterator;
        }
        
    };

}
#endif