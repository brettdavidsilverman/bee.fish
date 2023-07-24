#ifndef BEE_FISH__DATABASE__BRANCH_HPP
#define BEE_FISH__DATABASE__BRANCH_HPP
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
         out <<
            "{" <<
               "\"left\":" <<  branch._left << ", " <<
               "\"right\": " << branch._right <<
            "}";

         return out;
      }
      
      inline static const Index Root = 0;
         
   };
   
   
}

#endif