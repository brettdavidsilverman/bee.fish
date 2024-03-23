#ifndef BEE_FISH__DATABASE__INDEX_HPP
#define BEE_FISH__DATABASE__INDEX_HPP

#include <cmath>
#include <atomic>
#include "Config.hpp"
#include "File.hpp"

using namespace std;

namespace BeeFishDatabase {

   typedef Size Index;
   typedef std::atomic<Index> AtomicIndex;
   

}

#endif