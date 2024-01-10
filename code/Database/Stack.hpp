#ifndef BEE_FISH__DATABASE__STACK_HPP
#define BEE_FISH__DATABASE__STACK_HPP

#include "Path.hpp"

using namespace std;
using namespace BeeFishPowerEncoding;

namespace BeeFishDatabase {
    
   struct StackValue {
   public:
      Path _path;
      bool _bit = false;
      
      StackValue() {
         _bit = false;
      }
      
      StackValue(const Path& path, bool bit) :
          _path(path),
          _bit(bit)
      {
      }
      
      StackValue(const StackValue& source) :
          _path(source._path),
          _bit(source._bit)
      {
      }
      /*
      virtual ~StackValue() {
         if (_path) {
            delete _path;
            _path = nullptr;
         }
      */
   };

   class Stack :
      public PowerEncoding,
      public vector<StackValue>
   {
   protected:
      Size _index {0};
      long int _count {0};

   public:
      
      Stack()
      {
      }
     
      virtual ~Stack() {
      }

      virtual bool peekBit() const
      override
      {
         return (*this)[_index]._bit;
      }

      virtual bool readBit()
      override
      {

         bool value = peekBit();

         PowerEncoding::readBit();

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
         PowerEncoding::writeBit(bit);
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
      {
         return _count;
      }
      

      friend ostream& operator << (ostream& out, const Stack& stack)
      {
         int index = 0;
        
         for (auto value : stack)
         {
            out << value._path << ":" 
                << value._bit
                << endl;
         }

         return out;

      }
 
      StackValue last() {
         size_t size = vector<StackValue>::size();
         assert(size);
         return (*this)[size - 1];
      }

   };
   
}

#endif

