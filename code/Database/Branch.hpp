#ifndef BEE_FISH__DATABASE__BRANCH_HPP
#define BEE_FISH__DATABASE__BRANCH_HPP
#include "Index.hpp"

using namespace std;

namespace BeeFishDatabase
{

   struct Branch
   {
      Index _parent;
      Index _left;
      Index _right;
      Index _dataIndex;
      
      bool isDeadEnd()
      {
         return _left  == 0 &&
                _right == 0;
      }
      
      friend ostream& operator << 
      (ostream& out, Branch& branch)
      {
         out <<
            "{\"parent\": " << branch._parent << ", " <<
            "\"left\": " <<  branch._left << ", " <<
            "\"right\": " << branch._right << "}";

         return out;
      }
      
      inline static const Index Root = 0;
         
   };
   
   
}

#endif