#ifndef BEE_FISH__DATABASE__ITERATOR_HPP
#define
#include "Path.hpp"

namespace BeeFishDatabase {
   // iterator class is parametrized by pointer type
   template <typename PointerType>
   class MyIterator {
      // iterator class definition goes here
   };


   typedef MyIterator<int*> iterator_type;
   typedef MyIterator<const int*> const_iterator_type;
}

#endif