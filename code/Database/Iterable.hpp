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
        public JoinPathBase<T>
    {

    protected:
        Path _path;
        
    public:


        Iterable(Path path) : 
            _path(path)
        {
        }
    
        Iterable(Database& db) :
            Iterable(Path(db))
        {
        }
        
        virtual ~Iterable()
        {
        }
        
        template<typename Key>
        Iterable<T> operator [] (const Key& key)
        {

            Path path(_path);

            path << key;

            path.unlock();

            return path;
        }
        
        Iterable<T> operator [] (const char* key)
        {
            return (*this)[BString(key)];
        }
        
    protected:
        
        virtual bool canGoLeft() const
        {
            return _path.canGoLeft();
        }
        
        virtual bool canGoRight() const
        {
            return _path.canGoRight();
        }
        
        
        virtual void goLeft()
        {
            _path.goLeft();
        }
        
        virtual void goRight()
        {
            _path.goRight();
        }
        
        virtual void goUp() 
        {
            _path.goUp();
        }
        
        virtual bool isDeadEnd() const
        {
            return _path.isDeadEnd();
        }
        
    public:
        
        // Container methods to get iterators
        virtual PathBase::PathIterator<T> begin() {
            return PathBase::PathIterator<T>(&_path);
        }
    
        
    };

}
#endif