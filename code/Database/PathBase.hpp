#ifndef BEE_FISH__DATABASE__PATH_BASE_HPP
#define BEE_FISH__DATABASE__PATH_BASE_HPP

#include "Stack.hpp"

namespace BeeFishDatabase {

    class PathBase
    {
    protected:
        Index _index = 0;
    public:

        PathBase() {
        }
        
        
        virtual bool canGoLeft() const = 0;
        virtual bool canGoRight() const = 0;
        
        
        virtual void goLeft() = 0;
        virtual void goRight() = 0;
        virtual void goUp() = 0;
        
        virtual bool isDeadEnd() const
        {
            return not (
                canGoLeft() or
                canGoRight()
            );
        }
        
        
        
    public:

        bool min(
            Stack& stack,
            Index index
        )
        {
            _index = index;
            bool bit;
            
            while (!isDeadEnd())
            {
                
                
                if (canGoLeft()) {
                    bit = 0;
                    goLeft();
                    
                }
                else if (canGoRight()) {
                    bit = 1;
                    goRight();
                }
                else {
                    break;
                }
                
                stack.push_back(bit);
                
                if (stack.count() == 0)
                    return true;

            }
            
            
            return (stack.count() == 0);
            
        }

        
        bool max(
            Stack& stack,
            Index index
        )
        {
            _index = index;
            bool bit;
            while (!isDeadEnd())
            {
                
                
                if (canGoRight()) {
                    bit = 1;
                    goRight();
                }
                else if (canGoLeft()) {
                    bit = 0;
                    goLeft();
                }
                else
                    break;
                    
                stack.push_back(
                    bit
                );

                if (stack.count() == 0)
                    return true;
                
            }
            
            return (stack.count() == 0);
        }
        
        
        
        

    public:
        template<typename T>
        bool next(Stack& stack, T& value) {

            // Algorithm:
            // from minimum branch
            // Up the tree until first
            // right with a left

            // Take that right, then
            // follow next minimum

            if (stack.size() == 0)
            {
                if (isDeadEnd())
                    return false;
                    
                if (min(stack, _index))
                {
                    stack.reset();
                    stack >> value;
                }
                return true;
            }
            
    
    
            // bool bit;
            bool found = false;

            // Up the tree until first right
            // that hasn't been taken yet
            bool bit;
            
            while(stack.size())
            {
                bit = stack.last();
                
                stack.pop_back();
                goUp();
                
                if (canGoRight() &&
                     bit == 0)
                {
                    goRight();
                    stack.push_back(1);
                    found = true;
                    break;
                }
            
            }

            if (!found) {
                return false;
            }

            // Follow the next min from
            // this right
            min(stack, _index);

            if (stack.count() != 0)
                return false;

            // Get this value
            stack.reset();
            stack >> value;
            
            return true;

        }
    
        template<typename T>
        class PathIterator {
        protected:
            PathBase* _path = nullptr; // The underlying pointer that the iterator wraps
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
            
            PathIterator(PathBase* path) :
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
        
    
    };

}
#endif