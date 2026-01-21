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
        public Path
    {

    public:

        Iterable(Path path) : 
            Path(path)
        {
        }
    
        Iterable(Database& db) :
            Path(Path(db))
        {
        }
        
        virtual ~Iterable()
        {
        }

    public:
        
        // Container methods to get iterators
        virtual PathBase::PathIterator<T> begin() {
            return PathBase::PathIterator<T>(this);
        }
    
        // Points one past the last element
        Path::PathIterator<T> end() { 
            PathBase::PathIterator<T> iterator;
            return iterator;
        }
        
    };

}
#endif