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
        
        inline Branch()
        {
            memset(this, 0, sizeof(Branch));
        }
        
        Branch(Index parent) : Branch()
        {
            _parent = parent;
        }
        
        Branch(const Branch& source)
        {
            memcpy(this, &source, sizeof(Branch));
        }


        inline bool isDeadEnd() const
        {
            return ( _left  == 0 ) &&
                     ( _right == 0 );
        }

        inline Variable getVariable() const {
            Variable var = BeeFishScript::Object{
                {"parent", (BeeFishScript::Integer)_parent},
                {"left", (BeeFishScript::Integer)_left},
                {"right", (BeeFishScript::Integer)_right},
                {"dataIndex", (BeeFishScript::Integer)_dataIndex},
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