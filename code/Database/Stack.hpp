#ifndef BEE_FISH__DATABASE__STACK_HPP
#define BEE_FISH__DATABASE__STACK_HPP

#include "Path.hpp"


namespace BeeFishDatabase {
using namespace std;
using namespace BeeFishPowerEncoding;

    class Stack :
        public PowerEncoding,
        public vector<bool>
    {
    private:
        Size _index = 0;

    public:
        Stack()
        {
        }
      
        virtual ~Stack() {
        }

        virtual bool peekBit() const
        {
            return (*this)[_index];
        }

        virtual bool readBit()
        override
        {

            bool value = peekBit();

            PowerEncoding::readBit();
            
            ++_index;

            return value;
        }

        virtual void writeBit(bool bit)
        override
        {
            PowerEncoding::writeBit(bit);
++_index;
        }

        void reset()
        {
            _index = 0;
            PowerEncoding::reset();
        }

        long int count() const
        {
            return _count;
        }
        

        

        Variable getVariable() const
        {
            Variable var = BeeFishScript::Object{};
            
            BeeFishScript::Array array;

            for (auto bit : *this)
            {
                Variable entry = BeeFishScript::Object{
                    {"bit", BeeFishScript::Integer(bit ? 1 : 0)}
                };

                array.push_back(entry);
            }

            var["bits"] = array;

            return var;

        }
 
        bool last() const {
            size_t size = vector<bool>::size();
            assert(size);
            return (*this)[size - 1];
        }
        
        

        virtual void push_back(bool bit)
        {
            if (bit)
                ++_count;
            else if (_count > 0)
                --_count;
                
            vector<bool>::push_back(
                bit
            );
        }
        
        virtual void pop_back()
        {
            bool bit = last();
            
            if (bit && _count > 0)
                --_count;
            else if (!bit)
                ++_count;
                
            vector<bool>::pop_back();
        }
        
        friend ostream& operator << (
            ostream& output,
            const Stack& stack
        )
        {
            for (auto const bit : stack)
            {
                output << bit;
            }
            
            output << endl;
            
            return output;
        }
        
        

    };
    
}

#endif
