#ifndef BEE_FISH__DATABASE__MIN_MAX_PATH_HPP
#define BEE_FISH__DATABASE__MIN_MAX_PATH_HPP

#include "Path.hpp"
#include "Stack.hpp"

using namespace std;
using namespace BeeFishPowerEncoding;

namespace BeeFishDatabase {

    class MinMaxPath :
        public Path
    {
    public:

        MinMaxPath( Database* database = nullptr,
                Index index = Branch::Root ) :
            Path(database, index)
            
        {
        }

        MinMaxPath( Database& database,
                Index index = Branch::Root ) :
            Path(database, index)
        {
        }

        MinMaxPath( const Path& source,
                Index index) :
            Path(source, index)
        {
        }
    
        MinMaxPath(const Path& source) :
            Path(source)
            
        {
            
        }
        
        template<typename T>
        MinMaxPath operator [] (const T& key)
        {

            Path path(*this);

            path << key;

            path.unlock();

            return path;
        }
        
        MinMaxPath operator [] (const char* key)
        {
            return (*this)[BString(key)];
        }

        
        
        
    };

}
#endif