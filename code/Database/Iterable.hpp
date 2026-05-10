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
        Stack _stack;
        Stack _saveStack;
        
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
            _stack.push_back(0);
        }
        
        virtual void goRight() 
        override
        {
            _path.goRight();
            _stack.push_back(1);
        }
        
        virtual void goUp()
        override
        {
            _path.goUp();
            _stack.pop_back();
        }
        
        virtual void save() 
        override
        {
            _path.save();
            _saveStack = _stack;
        }
        
        virtual void restore()
        {
            _path.restore();
            _stack = _saveStack;
        }
        
        BString toData()
        {
            return _stack.toData();
        }
        
        BString toKey()
        {
            return toData().toBase64();
        }
        

    public:
        
        // Container methods to get iterators
        virtual Iterator<T> begin() {
            return Iterator<T>(this);
        }
        
        virtual Iterator<T> begin(const BString& key) {
            
            Stack stack = Stack::fromData(key.fromBase64());
            
            for (const auto bit : stack)
            {
                if (bit == 0 && canGoLeft())
                    goLeft();
                else if (bit == 1 && canGoRight())
                    goRight();
                else {
                    throw runtime_error("Invalid query");
                }
            }
            
            return Iterator<T>(this, stack);
            
        }
    
        // Points one past the last element
        Iterator<T> end() { 
            Iterator<T> iterator;
            return iterator;
        }
        
        // Container methods to get reverse iterators
        virtual ReverseIterator<T> rbegin() {
            return ReverseIterator<T>(this);
        }
    
        // Points one past the first element
        ReverseIterator<T> rend() { 
            ReverseIterator<T> iterator;
            return iterator;
        }
        
    };

}
#endif