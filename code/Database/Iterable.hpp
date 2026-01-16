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
    
        virtual bool canGoLeft(const Branch& branch) const
        override
        {
            return MinMaxPath::canGoLeft(branch);
        }
        
        virtual bool canGoRight(const Branch& branch) const
        {
            return MinMaxPath::canGoRight(branch);
        }
        
        virtual void goLeft(const Branch& branch)
        {
            MinMaxPath::goLeft(branch);
        }
        
        virtual void goRight(const Branch& branch)
        {
            MinMaxPath::goRight(branch);
        }
        

    public:
        class PathIterator {
        public:
            // Iterator traits (required for STL compatibility in C++17 and earlier)
            using iterator_category = std::forward_iterator_tag;
            using value_type        = T;
            using difference_type   = std::ptrdiff_t;
            using pointer           = T*;
            using reference         = const T&;

            // Constructor
            PathIterator(Iterable* path, bool isEnd = false) :
                _path(path),
                _stack(*path)
            {
                _isEnd = isEnd || _path->isDeadEnd();
                if (!_isEnd)
                    _item = _path->min<T>(_stack);
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
                _isEnd = !_path->next<T>(_stack, _item);
                return *this;
            }

            // Postfix increment operator (++)
            PathIterator operator++(int) {
                PathIterator tmp = *this;
                ++(*this);
                return tmp;
            }

            // Comparison operators (== and !=)
            friend bool operator== (const PathIterator& a, const PathIterator& b)
            {
                return  (*(a._path) == *(b._path) &&
                        a._isEnd == b._isEnd &&
                        a._stack == b._stack);
            }
            friend bool operator!= (const PathIterator& a, const PathIterator& b)
            { 
                return (*(a._path) != *(b._path) ||
                        a._isEnd != b._isEnd ||
                        a._stack != b._stack);
            }

        protected:
            Iterable* _path; // The underlying pointer that the iterator wraps
            T _item;
            Stack _stack;
            bool _isEnd = false;
        };
        
    

        // Container methods to get iterators
        PathIterator begin() {
            return PathIterator(this);
        }
    
        // Points one past the last element
        PathIterator end() { 
            return PathIterator(this, true);
        }
    };

}
#endif