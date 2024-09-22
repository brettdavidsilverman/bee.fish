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

   protected:

      void min(
         Index index,
         Stack& stack
      ) const
      {
         Branch branch =
            getBranch(index);

         while (!branch.isDeadEnd())
         {
            bool bit = stack.last()._bit;

            index = 0;

            if (bit == 0) {
               index = branch._left;
            }
            else if (bit == 1) {
               index = branch._right;
            }
            else
               assert(false);

            branch = getBranch(index);

            if (branch._left)
               bit = 0;
            else if (branch._right)
               bit = 1;

            stack.push_back(
               StackValue(
                  index,
                  bit
               )
            );

            if (stack._count == 0)
               break;

         }
      }

      void max(
         Index index,
         Stack& stack
      ) const
      {
         Branch branch =
            getBranch(index);

         while (!branch.isDeadEnd())
         {
            bool bit = stack.last()._bit;
            index = 0;

            if (bit == 0) {
               index = branch._left;
            }
            else if (bit == 1) {
               index = branch._right;
            }
            else
               assert(false);

            branch = getBranch(index);
            
            bit = 0;

            if (branch._right)
               bit = 1;
            else if (branch._left)
               bit = 0;

            stack.push_back(
               StackValue(
                  index,
                  bit
               )
            );

            if (stack.count() == 0)
               break;
            
         }
      }
      
      void latest(
         Index index,
         Stack& stack
      ) const
      {
         Branch branch =
            getBranch(index);

         while (!branch.isDeadEnd())
         {
            bool bit = stack.last()._bit;
            index = 0;

            if (bit == 0) {
               index = branch._left;
            }
            else if (bit == 1) {
               index = branch._right;
            }
            else
               assert(false);

            branch = getBranch(index);
            
            if (branch._left && branch._right)
            {
               if (branch._right > branch._left) {
                  bit = 1;
               }
               else {
                  bit = 0;
               }
            }
            else if (branch._right) {
               bit = 1;
            }
            else if (branch._left) {
               bit = 0;
            }
            else
               assert(false);


            branch = getBranch(index);
            
            stack.push_back(
               StackValue(
                  index,
                  bit
               )
            );

            if (stack.count() == 0)
               break;
            
         }
      }
      

   public:

      template<typename T>
      T min(
         Stack& stack
      ) const
      {
         if (stack.size() == 0)
            stack.push_back(StackValue(_index, 1));

         min(_index, stack);
         T minimum = T();
         stack >> minimum;
         return minimum;
      }

      template<typename T>
      T max(
         Stack& stack
      ) const
      {
         if (stack.size() == 0)
            stack.push_back(StackValue(_index, 1));

         max(_index, stack);
         T maximum;
         stack >> maximum;
         return maximum;
      }
      
      template<typename T>
      T latest(
         Stack& stack
      ) const
      {
         if (stack.size() == 0)
            stack.push_back(StackValue(_index, 1));

         latest(_index, stack);
         T _latest;
         stack >> _latest;
         return _latest;
      }

      template<typename T>
      T min() const 
      {
         Stack stack(*this);
         T minimum = min<T>(stack);
         return minimum;
      }

      template<typename T>
      T max() const 
      {
         Stack stack(*this);
         T maximum = max<T>(stack);
         return maximum;
      }
      
      template<typename T>
      T latest() const 
      {
         Stack stack(*this);
         T _latest = latest<T>(stack);
         return _latest;
      }

      template<typename T>
      bool next(Stack& stack, T& value) {

         // Algorithm:
         // Up the tree until first
         // right with a left

         // Take that right, then
         // follow using min algo

         if (isDeadEnd())
            return false;

         if (stack.size() == 0)
         {
            value = min<T>(stack);
            return true;
         }
         
         StackValue entry;
         Index index;
         Branch branch;
         bool bit;
         bool found = false;

         // Up the tree until first right
         // that hasn't been taken yet
         
         while(stack.size())
         {
            entry = stack.last();
            bit = entry._bit;
            index = entry._index;
            branch = getBranch(index);

            if (
               branch._left &&
               branch._right &&
               bit == 0 )
            {
               stack[stack.size() - 1]._bit = 1;
               stack._count+=2;
               found = true;
               break;
            }
         
            
            stack.pop_back();

         }

         if (!found) {
            return false;
         }

         // Follow the next min from
         // this right
         min(index, stack);

         // Get this value
         stack.reset();
         stack >> value;

         return true;

      }

      template<typename T>
      T value() const
      {
         if (isDeadEnd())
            return T();
            
         return latest<T>();
      }

      
   };

}
#endif