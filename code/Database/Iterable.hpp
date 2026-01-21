#ifndef BEE_FISH__DATABASE__ITERABLE_HPP
#define BEE_FISH__DATABASE__ITERABLE_HPP

#include "Path.hpp"
#include "Stack.hpp"
#include <iostream>
#include <iterator> // Required for std::iterator_traits (C++17 and earlier)
#include <cstddef>  // Required for std::ptrdiff_t


using namespace std;
using namespace BeeFishPowerEncoding;

namespace BeeFishDatabase {
    
    template<typename T>
    class Iterable :
        public PathBase
    {
        PathBase& _path;
        
    public:

        Iterable(PathBase& path) : 
            _path(path)
        {
        }
        
        virtual ~Iterable()
        {
        }
        
    
        virtual bool canGoLeft() const 
        override
        {
            return _path.canGoLeft();
        }
        
        virtual bool canGoRight() const
        override
        {
            return _path.canGoRight();
        }
        
        
        virtual void goLeft() 
        override
        {
            _path.goLeft();
        }
        
        virtual void goRight() 
        override
        {
            _path.goRight();
        }
        
        virtual void goUp()
        override
        {
            _path.goUp();
        }
        
        virtual void save() 
        override
        {
            _path.save();
        }
        
        virtual void restore()
        {
            _path.restore();
        }

    public:
        
        // Container methods to get iterators
        virtual PathBase::PathIterator<T> begin() {
            return PathBase::PathIterator<T>(&_path);
        }
    
        // Points one past the last element
        Path::PathIterator<T> end() { 
            PathBase::PathIterator<T> iterator;
            return iterator;
        }
        
    };

}
#endif