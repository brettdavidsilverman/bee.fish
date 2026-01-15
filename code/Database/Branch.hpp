#ifndef BEE_FISH__DATABASE__BRANCH_HPP
#define BEE_FISH__DATABASE__BRANCH_HPP

#include <iomanip>
#include "../Script/Variable.hpp"
#include "Index.hpp"

using namespace std;

namespace BeeFishDatabase
{

    class Database;

    using namespace BeeFishScript;

    struct Branch
    {
        Index _parent;
        Index _left;
        Index _right;
        Index _dataIndex;

        
        bool  _locked;
        
        Branch()
        {
            memset(this, 0, sizeof(Branch));
        }
        
        Branch(const Branch& source) :
            _parent(source._parent),
            _left(source._left),
            _right(source._right),
            _dataIndex(source._dataIndex),
            _locked(source._locked)
        {
        }

        Branch& operator= (const Branch& rhs) 
        {
            _parent = rhs._parent;
            _left = rhs._left;
            _right = rhs._right;
            _dataIndex = rhs._dataIndex;
            _locked = rhs._locked;

            return *this;
        }

        bool isDeadEnd() const
        {
            return ( _left  == 0 ) &&
                     ( _right == 0 );
        }

        Variable getVariable() const {
            Variable var = BeeFishScript::Object{
                {"parent", (BeeFishScript::Integer)_parent},
                {"left", (BeeFishScript::Integer)_left},
                {"right", (BeeFishScript::Integer)_right},
                {"locked", (BeeFishScript::Boolean)_locked}
            };
            return var;
        }
        
        friend ostream& operator << 
        (ostream& out, const Branch& branch)
        {
            
            Variable var = branch.getVariable();

            out << var;

            return out;
        }

        inline static const Index Root = 0;
            
    };
    
    
}

#endif