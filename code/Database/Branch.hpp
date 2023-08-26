#ifndef BEE_FISH__DATABASE__BRANCH_HPP
#define BEE_FISH__DATABASE__BRANCH_HPP

#include <iomanip>
#include "Index.hpp"

using namespace std;

namespace BeeFishDatabase
{

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
      
      friend ostream& operator << 
      (ostream& out, const Branch& branch)
      {
         
         out << "["
             << setw(4) << branch._left
             << ", "
             << setw(4) << branch._right
             << "]";

         //if (branch._dataIndex)
         //   out << setw(4) << branch._dataIndex;

         return out;
      }
      
      inline static const Index Root = 0;
         
   };
   
   
}

#endif