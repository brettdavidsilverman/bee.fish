#ifndef BEE_FISH__QUERY__AND_PATH_HPP
#define BEE_FISH__QUERY__AND_PATH_HPP

#include "../Database/Database.hpp"
#include "JoinPathBase.hpp"

namespace BeeFishQuery {

using namespace std;
using namespace BeeFishPowerEncoding;
using namespace BeeFishDatabase;

    class AndPath :
        public JoinPathBase<JSONPath::Id>
    {
    protected:
        PathBase* _a;
        PathBase* _b;
        PathBase* _bounds;
        
    public:

        AndPath( PathBase* a,
                 PathBase* b,
                 PathBase* bounds) :
            _a(a),
            _b(b),
            _bounds(bounds)
        {
        }
   
        virtual ~AndPath()
        {
            delete _a;
            delete _b;
            delete _bounds;
        }
        
        
        virtual bool canGoLeft() const
        override
        {
            return
                _a->canGoLeft() and
                _b->canGoLeft() and
                _bounds->canGoLeft();
        }
        
        virtual bool canGoRight() const
        override
        {
            return
                _a->canGoRight() and
                _b->canGoRight() and
                _bounds->canGoRight();
        }
        
        virtual void goLeft()
        override
        {
            _a->goLeft();
            _b->goLeft();
            _bounds->goLeft();
    
        }
        
        virtual void goRight()
        override
        {
            _a->goRight();
            _b->goRight();
            _bounds->goRight();
        }
        
        virtual void goUp()
        override
        {
            _a->goUp();
            _b->goUp();
            _bounds->goUp();
        }

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
/*
        virtual bool isDeadEnd() const
        override
        {
            bool isDeadEnd =
            not (
                    (
                        _a->canGoLeft() and
                        _b->canGoLeft()
                    ) or
                    (
                        _a->canGoRight() and
                        _b->canGoRight()
                    )
                );
                
            return isDeadEnd;
                
        }
*/


    
    };

}
#endif