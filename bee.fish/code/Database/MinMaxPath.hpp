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
            MinMaxPath path(*this, index);
            
            bool bit;

            if (branch._left) {
               index = branch._left;
               bit = 0;
            }
            else if (branch._right) {
               index = branch._right;
               bit = 1;
            }

            stack.push_back(
                StackValue(
                   path,
                   bit
                )
            );

            branch = getBranch(index);
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
            MinMaxPath path(*this, index);

            bool bit;
         
            if (branch._right) {
               index = branch._right;
               bit = 1;
            }
            else if (branch._left) {
               index = branch._left;
               bit = 0;
            }

            stack.push_back(
               StackValue(
                  path,
                  bit
               )
            );

            branch = getBranch(index);
         }
      
      }

   public:
      template<typename T>
      T min(
         Stack& stack
      ) const
      {
         min(_index, stack);
         T minimum = T();
         stack >> minimum;
         assert(stack.count() == 0);
         return minimum;
      }

      template<typename T>
      T max(
         Stack& stack
      ) 
      {
         max(_index, stack);
         T maximum;
         stack >> maximum;
         return maximum;
      }

      template<typename T>
      T min() const {
         Stack stack;
         min(_index, stack);
         T minimum;
         stack >> minimum;
         return minimum;
      }

      template<typename T>
      T max() const {

         Stack stack;
         max(_index, stack);
         T maximum;
         stack >> maximum;
         return maximum;
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
            min(_index, stack);
            stack >> value;
            return true;
         }
         
         Branch branch;
         MinMaxPath path;
         StackValue entry;
         // Up the tree until first right
         do 
         {
            entry = stack.last();
            path = entry._path;
            branch = path.getBranch();
            stack.pop_back();
         }
         while (stack.size() && 
                ( !(branch._left &&
                     branch._right) ||
                entry._bit == 1) );

         if ( ! (branch._left and
                  branch._right) ||
              (entry._bit == 1) )
            return false;


         assert(branch._left && branch._right && entry._bit == 0);

         // Follow the next min from
         // this right
         stack.push_back(
             StackValue(path, 1)
         );
         
         min(branch._right, stack);

         // Get this value
         stack.reset();
         value = T();
         stack >> value;

         return true;

      }

      template<typename T>
      T value() const
      {
         if (isDeadEnd())
            return T();
            
         return min<T>();
      }

      
   };

}
#endif