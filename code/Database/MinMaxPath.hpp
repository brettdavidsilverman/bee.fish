#ifndef BEE_FISH__DATABASE__MIN_MAX_PATH_HPP
#define BEE_FISH__DATABASE__MIN_MAX_PATH_HPP

#include "Path.hpp"
#include "Stack.hpp"

using namespace std;
using namespace BeeFishPowerEncoding;

namespace BeeFishDatabase {

    class MinMaxPath :
        public Path
    {
    public:

        MinMaxPath( Database* database = nullptr,
                Index index = Branch::Root ) :
            Path(database, index)
            
        {
        }

        MinMaxPath( Database& database,
                Index index = Branch::Root ) :
            Path(database, index)
        {
        }

        MinMaxPath( const Path& source,
                Index index) :
            Path(source, index)
        {
        }
    
        MinMaxPath(const Path& source) :
            Path(source)
            
        {
            
        }
        
        template<typename T>
        MinMaxPath operator [] (const T& key)
        {

            Path path(*this);

            path << key;

            path.unlock();

            return path;
        }
        
        MinMaxPath operator [] (const char* key)
        {
            return (*this)[BString(key)];
        }

        template<typename T>
        T min()
        {
            T value;
            if (not min(value)) 
            {
                throw runtime_error("No minimum");
            }
            
            return value;
        }
        
        template<typename T>
        bool min(T& value)
        {
            Stack stack;
            MinMaxPath path(*this);
            
            if (path.min(stack, _index))
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
            T value;
            if (not max(value)) 
            {
                throw runtime_error("No maximum");
            }
            
            return value;
        }
        
        template<typename T>
        bool max(T& value)
        {
            Stack stack;
            MinMaxPath path(*this);
            
            if (path.max(stack, _index)) {
                stack.reset();
                stack >> value;
                return true;
            }
            
            return false;
        }
        
        
    protected:

        
        
        bool min(
            Stack& stack,
            Index index
        )
        {
            _index = index;
            
            while (!isDeadEnd())
            {
                bool bit;
                
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
        
    };

}
#endif