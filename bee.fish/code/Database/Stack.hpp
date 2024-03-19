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
            if (index++ == 0)
               out << value._path;
               
            out << value._bit;
         }
         
         return out;

      }
 
      StackValue last() {
         size_t size = vector<StackValue>::size();
         assert(size);
         return (*this)[size - 1];
      }
      
      virtual void push_back(const StackValue& value)
      {
         if (value._bit)
            ++_count;
         else
            --_count;
            
         vector<StackValue>::push_back(
            value
         );
      }
      
      virtual void pop_back()
      {
         StackValue value = (*this)[size() - 1];
         
         if (value._bit)
            --_count;
         else
            ++_count;
            
         vector<StackValue>::pop_back();
      }
 

   };
   
}

#endif

