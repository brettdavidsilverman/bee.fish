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
      /*
      friend ostream& operator << 
      (ostream& out, Branch& branch)
      {
         using namespace BeeFishBScript;

         Object object = {
            {"parent", (Number)branch._parent},
            {"left",   (Number)branch._left},
            {"right",  (Number)branch._right}
         };

         out << object;
          
         return out;
      }*/
      
      inline static const Index Root = 0;
         
   };
   
   
}

#endif