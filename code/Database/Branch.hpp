#ifndef BEE_FISH__DATABASE__BRANCH_HPP
#define BEE_FISH__DATABASE__BRANCH_HPP

#include <iomanip>
#include "../Script/Variable.hpp"
#include "Index.hpp"

using namespace std;

namespace BeeFishDatabase
{

   using namespace BeeFishScript;

   struct Branch
   {
      Index _left;
      Index _right;
      Index _dataIndex;
      
      bool isDeadEnd() const
      {
         return ( _left  == 0 ) &&
                ( _right == 0 );
      }

      Variable getVariable() const {
         Variable var = BeeFishScript::Map{
            {"left", (Number)_left},
            {"right", (Number)_right}
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