#ifndef BEE_FISH__QUERY__JOIN_PATH_BASE_HPP
#define BEE_FISH__QUERY__JOIN_PATH_BASE_HPP

#include "../Database/Database.hpp"

namespace BeeFishQuery {

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
   
        virtual PathBase::PathIterator<T> begin() 
        {
            return PathBase::PathIterator<T>(this);
        }
    
        // Points one past the last element
        PathBase::PathIterator<T> end()
        { 
            PathBase::PathIterator<T> iterator;
            return iterator;
        }
        
    };

}
#endif