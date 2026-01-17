#ifndef BEE_FISH__DATABASE__ITERABLE_HPP
#define BEE_FISH__DATABASE__ITERABLE_HPP

#include "MinMaxPath.hpp"
#include "Stack.hpp"
#include <iostream>
#include <iterator> // Required for std::iterator_traits (C++17 and earlier)
#include <cstddef>  // Required for std::ptrdiff_t


using namespace std;
using namespace BeeFishPowerEncoding;

namespace BeeFishDatabase {
    
    template<typename T>
    class Iterable :
        public MinMaxPath
    {

    
    public:


        Iterable(const Path& source) :
            MinMaxPath(source)
         
        {
        }
    
        

    public:
        class PathIterator {
        protected:
            Iterable* _path = nullptr; // The underlying pointer that the iterator wraps
            T _item;
            Stack _stack;
            bool _isEnd = true;
            
        public:
            // Iterator traits (required for STL compatibility in C++17 and earlier)
            using iterator_category = std::forward_iterator_tag;
            using value_type        = T;
            using difference_type   = std::ptrdiff_t;
            using pointer           = T*;
            using reference         = const T&;

            // Constructor
            PathIterator() {
            }
            
            PathIterator(Iterable* path) :
                _path(path)
            {
                _isEnd = !_path->next<T>(_stack, _item);
            }
            
            
            // Dereference operator (*)
            reference operator*() const
            {
                return _item;
            }

            // Arrow operator (->)
            pointer operator->() const 
            {
                return &_item; 
            }

            // Prefix increment operator (++)
            PathIterator& operator++() {
                assert(!_isEnd);
                _isEnd = not
                    _path->next<T>(
                        _stack,
                        _item
                    );
                return *this;
            }

            // Postfix increment operator (++)
            PathIterator operator++(int) {
                PathIterator tmp = *this;
                ++(*this);
                return tmp;
            }

            friend bool operator == (
                const PathIterator& a,
                const PathIterator& b
            )
            {
                return  (a._isEnd == b._isEnd);
            }
            
            friend bool operator != (
                const PathIterator& a,
                const PathIterator& b
            )
            { 
                return (a._isEnd != b._isEnd);
            }

        
        };
        
    

        // Container methods to get iterators
        PathIterator begin() {
            return PathIterator(this);
        }
    
        // Points one past the last element
        PathIterator end() { 
            PathIterator iterator;
            return iterator;
        }
        
    };

}
#endif