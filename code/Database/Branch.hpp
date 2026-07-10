#ifndef BEE_FISH__DATABASE__BRANCH_HPP
#define BEE_FISH__DATABASE__BRANCH_HPP

#include <iomanip>
#include <iostream>
#include <atomic>
#include <boost/interprocess/managed_shared_memory.hpp>
#include "../Script/Variable.hpp"
#include "../Size.hpp"

namespace bip = boost::interprocess;

using namespace std;

namespace BeeFishDatabase
{

    class Database;
    

    using namespace BeeFishScript;

    struct Branch
    {
        Index _parent = 0;
        Index _left = 0;
        Index _right = 0;
        Index _dataIndex = 0;
        
        inline Branch()
        {
        }
        
        Branch(Index parent) :
            _parent(parent)
        {
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
                {"dataIndex", (BeeFishScript::Integer)_dataIndex}
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
    
    /*
int main() {
    
        // 3. Perform atomic operations
    // fetch_add returns the old value; the increment itself is strictly atomic
    int old_val = shared_atomic->fetch_add(1, std::memory_order_relaxed);
    
    std::cout << "Previous value was: " << old_val << std::endl;
    std::cout << "Current value is: " << shared_atomic->load() << std::endl;

    // 4. (Optional) Remember to remove the segment when completely finished with IPC
    // bip::shared_memory_object::remove(shm_name);

    return 0;
}

    */
    
}

#endif