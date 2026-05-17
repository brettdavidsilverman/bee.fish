#ifndef BEE_FISH__DATABASE__PATH_BASE_HPP
#define BEE_FISH__DATABASE__PATH_BASE_HPP

#include "../power-encoding/stack.h"

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
       // virtual void save() = 0;
       // virtual void restore() = 0;
        virtual PathBase* copy() const = 0;
        
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
                if (isDeadEnd())
                    return false;
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
                            break;
                        }
                
                        if (stack.count() == 0)
                            return true;
                    }
                    
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
                if (isDeadEnd())
                    return false;
                   
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
                    
                    // Follow to next max
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
                            break;
                        }
                
                        if (stack.count() == 0)
                            return true;
                
                    }
                }
            
            }


            return false;
            
        }
        
        template<typename T>
        bool previous(Stack& stack, T& item)
        {
            bool result = previous(stack);
            if (result)
            {
                stack.reset();
                stack >> item;
            }
            return result;
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
                   return next(stack);
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
                    return previous(stack);
                }
                
                if (stack.count() == 0)
                    return true;
                
            }
            
            return false;
        }
        
        
    public:
        template<typename T>
        T min() const
        {
            Stack stack;
            
            PathBase* path = copy();
            
            bool result = path->goToMin(stack);
            
            delete path;
            
            if (!result)
            {
                throw runtime_error("No minimum");
            }
            
            T value;
            stack.reset();
            stack >> value;
                
            return value;
            
        }
        
        template<typename T>
        bool min(T& value) const
        {
            Stack stack;
            
            PathBase* path = copy();
            
            bool result = path->goToMin(stack);
            
            delete path;
            
            if (result)
            {
                stack.reset();
                stack >> value;
                return true;
            }

            return false;
        }
        
        template<typename T>
        T max() const
        {
            Stack stack;
            
            PathBase* path = copy();
            
            bool result = path->goToMax(stack);
            
            delete path;
            
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
        bool max(T& value) const
        {
            Stack stack;
            
            PathBase* path = copy();
            
            bool result = path->goToMax(stack);
            
            delete path;
            
            if (result)
            {
                stack.reset();
                stack >> value;
                return true;
            }

            return false;
        }

    
    };

}
#endif