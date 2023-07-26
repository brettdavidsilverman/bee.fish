#ifndef BEE_FISH__DATABASE__STACK_HPP
#define BEE_FISH__DATABASE__STACK_HPP
#include <vector>
#include "Database.hpp"

using namespace std;
using namespace BeeFishPowerEncoding;

namespace BeeFishDatabase {


   template <class Encoding = Database::Encoding>
   class Stack :
      public Encoding,
      public vector< Branch >
   {
   protected:
      Size _index {0};
      long int _count {0};

   public:
      enum class Aggregate {
         MIN,
         MAX
      } _aggregate = Aggregate::MIN;

      Stack()
      {
      }
     
      virtual bool peekBit() const
      override
      {
         Branch branch =
            (*this)[_index];
      
         if (_aggregate ==
            Aggregate::MIN)
         {
            if (branch._left)
               return 0;

            if (branch._right)
               return 1;
         }
         else if (_aggregate ==
                  Aggregate::MAX)
         {
            if (branch._right)
               return 1;

            if (branch._left)
               return 0;
         }
      
         assert(false);
      }

      virtual bool readBit()
      override
      {

         bool value = peekBit();

         Encoding::readBit();

         if (value)
            ++_count;
         else
            --_count;

         ++_index;

         return value;
      }

      virtual void writeBit(bool bit)
      override
      {
         Encoding::writeBit(bit);
         if (bit)
            ++_count;
         else
            --_count;
      }

      void reset()
      {
         _index = 0;
         _count = 0;
      }

      virtual long int count() const
      override
      {
         return _count;
      }

      Branch last()
      {
         assert(size() > 0);
         Branch branch =
            (*this)[size() - 1];
         return branch;
      }

      friend ostream& operator << (ostream& out, const Stack& stack)
      {
         int index = 0;
        
         for (auto branch : stack)
         {
            out << branch;
            if (branch._left and branch._right)
               out << "****";
            out << endl;
         
         }

         return out;

      }

   };

}

#endif