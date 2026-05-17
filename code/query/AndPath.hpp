#ifndef BEE_FISH__QUERY__AND_PATH_HPP
#define BEE_FISH__QUERY__AND_PATH_HPP

#include "../Database/Database.hpp"

namespace BeeFishQuery {

using namespace std;
using namespace BeeFishPowerEncoding;
using namespace BeeFishDatabase;

    class AndPath :
        public PathBase
    {
    protected:
        PathBase* _a;
        PathBase* _b;
        
    public:

        AndPath( 
            const PathBase& a,
            const PathBase& b
        ) :
            _a(a.copy()),
            _b(b.copy())
        {
        }
        
        AndPath(const AndPath& source) :
            _a(source._a->copy()),
            _b(source._b->copy())
        {
        }
            
   
        virtual ~AndPath()
        {
            delete _a;
            delete _b;
        }
        
        
        virtual PathBase* copy() const
        override
        {
            return new AndPath(*this);
                
        }
        
        virtual bool canGoLeft() const
        override
        {
            return
                _a->canGoLeft() and
                _b->canGoLeft();
        }
        
        virtual bool canGoRight() const
        override
        {
            return
                _a->canGoRight() and
                _b->canGoRight();
        }
        
        virtual void goLeft()
        override
        {
            _a->goLeft();
            _b->goLeft();
    
        }
        
        virtual void goRight()
        override
        {
            _a->goRight();
            _b->goRight();
        }
        
        virtual void goUp()
        override
        {
            _a->goUp();
            _b->goUp();
        }
/*
        virtual void save()
        override
        {
            _a->save();
            _b->save();
            _bounds->save();
        }
        
        virtual void restore()
        override
        {
            _a->restore();
            _b->restore();
            _bounds->restore();
        }
    */
    };

}
#endif