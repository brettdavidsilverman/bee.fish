#ifndef BEE_FISH__DATABASE__PATH_BASE_HPP
#define BEE_FISH__DATABASE__PATH_BASE_HPP

#include "Stack.hpp"

namespace BeeFishDatabase {

    class PathBase
    {
    public:
        
        PathBase()
        {
        }
        
        virtual ~PathBase()
        {
        }
        
        
        virtual bool canGoLeft() const = 0;
        virtual bool canGoRight() const = 0;
        
        
        virtual void goLeft() = 0;
        virtual void goRight() = 0;
        virtual void goUp() = 0;
        virtual void save() = 0;
        virtual void restore() = 0;
        
        virtual bool isDeadEnd() const
        {
            return not (
                canGoLeft() or
                canGoRight()
            );
        }
        
        
        
        template<typename T>
        bool next(Stack& stack, T& item)
        {
            bool result = next(stack);
            if (result)
            {
                stack.reset();
                stack >> item;
            }
            return result;
        }
        
        bool next(Stack& stack) {

            // Algorithm:
            // from minimum branch
            // Up the tree until first
            // right from a left

            // Take that right, then
            // follow next minimum

            if (stack.size() == 0)
            {
                return goToMin(stack);
            }
            
    
    
            // Up the tree until first right
            // that hasn't been taken yet
            bool bit;
            
            while(stack.size())
            {
                bit = stack.last();
                
                // Up the stack
                stack.pop_back();

                goUp();
                
                // First right from a left
                if (canGoRight() &&
                     bit == 0)
                {
                    goRight();
                    stack.push_back(1);
                    // Follow min
                    return goToMin(stack);
                }
            
            }

            return false;


        }
        
        bool previous(Stack& stack) {

            // Algorithm:
            // from maximum branch
            // Up the tree until first
            // left from a right

            // Take that left, then
            // follow next max

            if (stack.size() == 0)
            {
                return goToMax(stack);
            }
            
    
            // Up the tree until first left
            // that hasn't been taken yet
            bool bit;
            
            while(stack.size())
            {
                bit = stack.last();
                
                stack.pop_back();
                goUp();
                
                if (canGoLeft() &&
                     bit == 1)
                {
                    goLeft();
                    stack.push_back(0);
                    return goToMax(stack);
                }
            
            }


            return false;


        }
    public:
        virtual bool goToMin(
            Stack& stack
        )
        {

            while (1)
            {
                
                if (canGoLeft()) {
                    stack.push_back(0);
                    goLeft();
                    
                }
                else if (canGoRight()) {
                    stack.push_back(1);
                    goRight();
                }
                else {
                    return false;
                }
                
                if (stack.count() == 0)
                    return true;
            }
            
            return false;

        }

        
        virtual bool goToMax(
            Stack& stack
        )
        {
            while (1)
            {
                
                if (canGoRight()) {
                    stack.push_back(1);
                    goRight();
                }
                else if (canGoLeft()) {
                    stack.push_back(0);
                    goLeft();
                }
                else {
                    return false;
                }
                
                if (stack.count() == 0)
                    return true;
                
            }
            
            return false;
        }
        
        
    public:
        template<typename T>
        T min()
        {
            Stack stack;
            
            save();
            bool result = goToMin(stack);
            restore();
            
            if (result)
            {
                T value;
                stack.reset();
                stack >> value;
                return value;
                
            }
            
            throw runtime_error("No minimum");
        }
        
        template<typename T>
        bool min(T& value)
        {
            Stack stack;
            save();
            bool result = goToMin(stack);
            restore();
            if (result)
            {
                stack.reset();
                stack >> value;
                return true;
            }

            return false;
        }
        
        template<typename T>
        T max()
        {
            Stack stack;
            
            save();
            bool result = goToMax(stack);
            restore();
            
            if (result)
            {
                T value;
                stack.reset();
                stack >> value;
                return value;
                
            }
            
            throw runtime_error("No maximum");
        }
        
        template<typename T>
        bool max(T& value)
        {
            Stack stack;
            
            save();
            bool result = goToMax(stack);
            restore();
            
            if (result)
            {
                stack.reset();
                stack >> value;
                return true;
            }

            return false;
        }
        

        
    public:
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
                tmp.save();
                ++(*this);
                tmp.restore();
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
        
        
        
        
    public:


    
    };

}
#endif