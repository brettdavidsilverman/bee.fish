#ifndef BEE_FISH__DATABASE__PATH_ITERATOR_HPP
#define BEE_FISH__DATABASE__PATH_ITERATOR_HPP

#include "Path.hpp"
#include "Stack.hpp"
#include <iostream>
#include <iterator> // Required for std::iterator_traits (C++17 and earlier)
#include <cstddef>  // Required for std::ptrdiff_t




using namespace std;
using namespace BeeFishPowerEncoding;

namespace BeeFishDatabase {
    
    template<typename T>
    class JoinPath :
        public MinMaxPath
    {
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
            PathIterator(JoinPath* path, bool isEnd = false) :
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
            JoinPath* _path; // The underlying pointer that the iterator wraps
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
    
   
   
    public:

        JoinPath( Database* database = nullptr,
            Index index = Branch::Root ) :
            MinMaxPath(database, index)
        {
        }

        JoinPath( Database& database,
            Index index = Branch::Root ) :
            MinMaxPath(database, index)
        {
        }

        JoinPath( const Path& source,
              Index index) :
              MinMaxPath(source, index)
        {
        }
   
        JoinPath(const Path& source) :
            MinMaxPath(source)
         
        {
         
        }
      

        

    
    };

}
#endif