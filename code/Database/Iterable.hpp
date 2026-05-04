#ifndef BEE_FISH__DATABASE__ITERABLE_HPP
#define BEE_FISH__DATABASE__ITERABLE_HPP

#include <iostream>
#include <iterator>
#include <cstddef> 
#include "../power-encoding/stack.h"
#include "Path.hpp"
#include "Iterator.hpp"
#include "ReverseIterator.hpp"

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
        virtual Iterator<T> begin() {
            return Iterator<T>(this);
        }
    
        // Points one past the last element
        Iterator<T> end() { 
            Iterator<T> iterator;
            return iterator;
        }
        
        // Container methods to get iterators
        virtual ReverseIterator<T> rbegin() {
            return ReverseIterator<T>(this);
        }
    
        // Points one past the last element
        ReverseIterator<T> rend() { 
            ReverseIterator<T> iterator;
            return iterator;
        }
        
    };

}
#endif