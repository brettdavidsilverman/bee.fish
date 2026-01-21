#ifndef BEE_FISH__QUERY__OR_PATH_HPP
#define BEE_FISH__QUERY__OR_PATH_HPP

#include "../Database/Database.hpp"

#include "JoinPathBase.hpp"

namespace BeeFishQuery {

using namespace std;
using namespace BeeFishPowerEncoding;
using namespace BeeFishDatabase;

    class OrPath :
        public PathBase
    {
    protected:
        PathBase* _a;
        PathBase* _b;
        bool _aEnded;
        bool _bEnded;
        
    public:

        OrPath(PathBase* a,
               PathBase* b) :
            _a(a),
            _b(b)
        {
            _aEnded = _a->isDeadEnd();
            _bEnded = _b->isDeadEnd();
        }
        
        virtual ~OrPath() {
            delete _a;
            delete _b;
        }
   
        
        virtual bool canGoLeft() const
        override
        {
            return
                _a->canGoLeft() or
                _b->canGoLeft();
        }
        
        virtual bool canGoRight() const
        override
        {
            return
                _a->canGoRight() or
                _b->canGoRight();
        }
        
        virtual void goLeft()
        override
        {
            if (!_aEnded &&
                 _a->canGoLeft())
            {
                _a->goLeft();
            }
            else
                _aEnded = true;
                
            if (!_bEnded &&
                _b->canGoLeft())
            {
                _b->goLeft();
            }
            else
                _bEnded = true;
                
        }
        
        virtual void goRight()
        override
        {
            if (!_aEnded &&
                 _a->canGoRight())
            {
                _a->goRight();
            }
            else
                _aEnded = true;
                
            if (!_bEnded &&
                _b->canGoRight())
            {
                _b->goRight();
            }
            else
                _bEnded = true;
                
        }
        
        virtual void goUp()
        override
        {
            assert(false);
        }
        
        virtual void save()
        {
            _a->save();
            _b->save();
        }
        
        virtual void restore()
        {
            _a->restore();
            _b->restore();
        }
        
    
    };

}
#endif