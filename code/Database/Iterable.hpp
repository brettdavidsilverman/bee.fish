#ifndef BEE_FISH__DATABASE__ITERABLE_HPP
#define BEE_FISH__DATABASE__ITERABLE_HPP

#include <iostream>
#include <iterator>
#include <cstddef>
#include "../power-encoding/stack.h"
#include "Path.hpp"


using namespace std;
using namespace BeeFishPowerEncoding;

namespace BeeFishDatabase {

template<typename T>
class Iterable :
    public PathBase
{
public:
    PathBase* _path;
    Stack _stack;
    Stack _saveStack;

public:

    Iterable(const PathBase& path) :
        _path(path.copy())
    {
    }

    Iterable(const Iterable& source) 
    {
        if (source._path)
            _path = source._path->copy();
            
        _stack = source._stack;
        _saveStack = source._saveStack;

    }
    
    Iterable& operator = (const Iterable& source)
    {
        if (source._path)
            _path = source._path->copy();
        else
            _path = nullptr;
            
        _stack = source._stack;
        _saveStack = source._saveStack;
        
        return *this;
    }
    
    

    virtual ~Iterable()
    {
        if (_path)
            delete _path;
    }

    virtual PathBase* copy() const
    override
    {
        return new Iterable<T>(*this);
    }

    virtual bool canGoLeft() const
    override
    {
        return _path->canGoLeft();
    }

    virtual bool canGoRight() const
    override
    {
        return _path->canGoRight();
    }


    virtual void goLeft()
    override
    {
        _path->goLeft();
        _stack.push_back(0);
    }

    virtual void goRight()
    override
    {
        _path->goRight();
        _stack.push_back(1);
    }

    virtual void goUp()
    override
    {
        _path->goUp();
        _stack.pop_back();
    }
    /*
            virtual void save()
            override
            {
                assert(false);
                _path->save();
                _saveStack = _stack;
            }

            virtual void restore()
            {
                _path->restore();
                _stack = _saveStack;
            }
            */
    BString toData()
    {
        return _stack.toData();
    }

    BString toKey()
    {
        return toData().toBase64();
    }


public:
    class Iterator {
    public:
        const Iterable<T>* _iterable = nullptr;
        PathBase* _path = nullptr;
        T _item;
        Stack _stack;
        bool _isEnd = true;

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type        = T;
        using difference_type   = std::ptrdiff_t;
        using pointer           = T*;
        using reference         = const T&;

        // Constructor
        Iterator() {
        }


        Iterator(const Iterable& iterable) :
            _iterable(&iterable)
            
        {

            if (iterable._path) {
                _path = iterable._path->copy();
                
                _isEnd = !_path->next<T>(_stack, _item);
            }
        }

        Iterator(const Iterable& iterable, const Stack& stack) :
            _iterable(&iterable),
            _path(iterable._path->copy()),
            _stack(stack)
        {
            _isEnd = !_path->next<T>(_stack, _item);

        }

        Iterator(const Iterator& source) :
            _iterable(source._iterable),
            _item(source._item),
            _stack(source._stack),
            _isEnd(source._isEnd)
        {
            if (source._path) {
                _path = source._path->copy();
            }
        }
        
        Iterator& operator = (const Iterator& source)
        {
            _iterable = source._iterable;
            _item = source._item;
            _stack = source._stack;
            _isEnd = source._isEnd;
            if (_path)
                delete _path;
                
            if (source._path)
                _path = source._path->copy();
            else
                _path = nullptr;
                
            return *this;
        }
        

        virtual ~Iterator()
        {
            if (_path)
                delete _path;
        }


        // Dereference operator (*)
        reference operator*() const
        {
            return _item;
        }

        // Arrow operator (->)
        pointer operator->()
        {
            return &_item;
        }

        // Prefix increment operator (++)
        Iterator& operator++() {
            assert(!_isEnd);
            _isEnd = not
                     _path->next<T>(
                         _stack,
                         _item
                     );
            return *this;
        }

        // Postfix increment operator (++)
        Iterator operator++(int) {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        /*

        void save()
        {
            _iterable->save();
        }

        void restore()
        {
            _iterable->restore();
        }
        */
        friend bool operator == (
            const Iterator& a,
            const Iterator& b
        )
        {
            return  (a._isEnd == b._isEnd);
        }

        friend bool operator != (
            const Iterator& a,
            const Iterator& b
        )
        {
            return (a._isEnd != b._isEnd);
        }


    };

    class ReverseIterator {
    public:
        const Iterable<T>* _iterable = nullptr;
        PathBase* _path = nullptr;
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
        ReverseIterator() {
        }

        ReverseIterator(const Iterable& iterable) :
            _iterable(&iterable)
            
        {
            if (iterable._path) {
                _path = iterable._path->copy();
                
                _isEnd = !_path->previous<T>(_stack, _item);
            }
        }
        
        ReverseIterator(const Iterator& source) :
            _iterable(source._iterable),
            _item(source._item),
            _stack(source._stack),
            _isEnd(source._isEnd)
        {
            if (source._path)
                _path = source._path->copy();
        }
        
        ReverseIterator& operator = (const ReverseIterator& source)
        {
            _iterable = source._iterable;
            _item = source._item;
            _stack = source._stack;
            _isEnd = source._isEnd;
            
            if (source._path)
                _path = source._path->copy();
            else
                _path = nullptr;
                
            return *this;
        }
        
        virtual ~ReverseIterator()
        {
            if (_path)
                delete _path;
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
        ReverseIterator& operator++() {
            assert(!_isEnd);
            _isEnd = not
                     _path->previous<T>(
                         _stack,
                         _item
                     );
            return *this;
        }

        // Postfix increment operator (++)
        ReverseIterator operator++(int) {
            ReverseIterator tmp = *this;
            ++(*this);
            return tmp;
        }

        friend bool operator == (
            const ReverseIterator& a,
            const ReverseIterator& b
        )
        {
            return  (a._isEnd == b._isEnd);
        }

        friend bool operator != (
            const ReverseIterator& a,
            const ReverseIterator& b
        )
        {
            return (a._isEnd != b._isEnd);
        }


    };



    // Container methods to get iterators
    virtual Iterator begin() const {
        return Iterator(*this);
    }

    virtual Iterator begin(const BString& key) const {

        Stack stack = Stack::fromData(key.fromBase64());
        Iterable<T> iterable(*this);
        for (const auto bit : stack)
        {
            if (bit == 0 && iterable.canGoLeft())
                iterable.goLeft();
            else if (bit == 1 && canGoRight())
                iterable.goRight();
            else {
                throw runtime_error("Invalid query");
            }
        }

        return Iterator(*this, stack);

    }

    // Points one past the last element
    Iterator end() const {
        return Iterator();
    }

    // Container methods to get reverse iterators
    virtual ReverseIterator rbegin() const {
        return ReverseIterator(*this);
    }

    // Points one past the first element
    ReverseIterator rend() const {
        ReverseIterator iterator;
        return iterator;
    }

};

}
#endif