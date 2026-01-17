#ifndef BEE_FISH__DATABASE__JOIN_PATH_BASE_HPP
#define BEE_FISH__DATABASE__JOIN_PATH_BASE_HPP

#include "PathBase.hpp"

namespace BeeFishDatabase {

using namespace std;
using namespace BeeFishPowerEncoding;
using namespace BeeFishDatabase;

    template<typename T>
    class JoinPathBase :
        public PathBase
    {
        
    public:

        JoinPathBase()
        {
        }
        
        virtual ~JoinPathBase()
        {
        }
   
        virtual PathIterator<T> begin() {
            return PathIterator<T>(this);
        }
    
        // Points one past the last element
        PathIterator<T> end() { 
            PathIterator<T> iterator;
            return iterator;
        }
        
        
    };

}
#endif