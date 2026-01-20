#ifndef BEE_FISH__DATABASE__JOIN_PATH_BASE_HPP
#define BEE_FISH__DATABASE__JOIN_PATH_BASE_HPP

#include "PathBase.hpp"
#include "Path.hpp"

namespace BeeFishDatabase {

using namespace std;
using namespace BeeFishPowerEncoding;
using namespace BeeFishDatabase;

    template<typename T>
    class JoinPathBase :
        public PathBase
    {
        
    public:

        JoinPathBase() : PathBase(0)
        {
        }
        
        virtual ~JoinPathBase()
        {
        }
   
        virtual Path::PathIterator<T> begin() {
            return Path::PathIterator<T>(this);
        }
    
        // Points one past the last element
        Path::PathIterator<T> end() { 
            Path::PathIterator<T> iterator;
            return iterator;
        }
        
        
    };

}
#endif