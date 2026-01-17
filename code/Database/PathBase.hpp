#ifndef BEE_FISH__DATABASE__PATH_BASE_HPP
#define BEE_FISH__DATABASE__PATH_BASE_HPP

namespace BeeFishDatabase {

    class PathBase
    {
    public:

        PathBase() {
        }
        
        
        virtual bool canGoLeft() const = 0;
        virtual bool canGoRight() const = 0;
        
        
        virtual void goLeft() = 0;
        virtual void goRight() = 0;
        virtual void goUp() = 0;
        
        virtual bool isDeadEnd() const
        {
            return not (
                canGoLeft() or
                canGoRight()
            );
        }
    
    };

}
#endif