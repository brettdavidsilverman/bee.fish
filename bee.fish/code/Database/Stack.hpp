#ifndef BEE_FISH__DATABASE__STACK_HPP
#define BEE_FISH__DATABASE__STACK_HPP

#include "Path.hpp"

using namespace std;
using namespace BeeFishPowerEncoding;

namespace BeeFishDatabase {
    
   struct StackValue {
   public:
      Index _index = 0;
      bool _bit = false;
      
      StackValue() {
      }
      
      StackValue(Index index, bool bit) :
          _index(index),
          _bit(bit)
      {
      }
      
      StackValue(const StackValue& source) :
          _index(source._index),
          _bit(source._bit)
      {
      }
      

   };

   class Stack :
      public PowerEncoding,
      public vector<StackValue>
   {
   public:
      Size _index = 0;
      long int _count = 0;
      
   public:
      const Path& _start;

      Stack(const Path& start) : _start(start)
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
         else if (_count > 0)
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
         else if (_count > 0)
            --_count;
      }

      void reset()
      {
         _index = 0;
         _count = 0;
      }

      long int count() const
      {
         return _count;
      }
      

      friend ostream& operator << (ostream& out, const Stack& stack) 
      {

         out << "StackStart: Index: " << stack._start.index()  << ", Count " << stack._count << endl;
         Index index = 0;

         for (auto value : stack)
         {
            Branch branch = stack._start.getBranch(value._index);
            out << index++
                << ":[" << value._index << "]"
                << "{" 
                  << branch._left << ", " << branch._right 
                << "}"
                << "[" << value._bit << "]"
                <<  endl;

         }

         return out;
      }

      Variable getVariable() const
      {
         Variable var = BeeFishScript::Object{
            {"start", (BeeFishScript::Number)_start.index()}
         };

         BeeFishScript::Array array;

         for (auto value : *this)
         {
            Variable entry = BeeFishScript::Object{
               {"index", BeeFishScript::Integer(value._index)},
               {"bit", BeeFishScript::Integer(value._bit ? 1 : 0)}
            };

            array.push_back(entry);
         }

         var["bits"] = array;

         return var;

      }
 
      StackValue last() const {
         size_t size = vector<StackValue>::size();
         assert(size);
         return (*this)[size - 1];
      }

      virtual void push_back(const StackValue& value)
      {
         if (value._bit)
            ++_count;
         else if (_count > 0)
            --_count;
            
         vector<StackValue>::push_back(
            value
         );
      }
      
      virtual void pop_back()
      {
         StackValue value = last();
         
         if (value._bit && _count > 0)
            --_count;
         else if (!value._bit)
            ++_count;
            
         vector<StackValue>::pop_back();
      }

      Branch getLastBranch() const
      {
         StackValue entry = last();
         return _start.getBranch(entry._index);
      }
 

   };
   
}

#endif
