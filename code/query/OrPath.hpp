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
        Index _aCount = 0;
        Index _bCount = 0;
        
        bool _saveAEnded;
        bool _saveBEnded;
        Index _saveACount = 0;
        Index _saveBCount = 0;
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
                (!_aEnded && _a->canGoLeft() ) or
                (!_bEnded && _b->canGoLeft());
        }
        
        virtual bool canGoRight() const
        override
        {
            return
                (!_aEnded && _a->canGoRight()) or
                (!_bEnded && _b->canGoRight());
        }
        
        virtual void goLeft()
        override
        {
            if (!_aEnded &&
                _a->canGoLeft())
            {
                _a->goLeft();
                ++_aCount;
            }
            else
                _aEnded = true;
                
            if (!_bEnded &&
                _b->canGoLeft())
            {
                _b->goLeft();
                ++_bCount;
            }
            else
                _bEnded = true;
                
        }
        
        virtual bool isDeadEnd() const
        override
        {
            return (_aEnded || _a->isDeadEnd()) &&
                   (_bEnded || _b->isDeadEnd());
        }
   
        
        virtual void goRight()
        override
        {
            if (!_aEnded &&
                 _a->canGoRight())
            {
                _a->goRight();
                ++_aCount;
            }
            else
                _aEnded = true;
                
            if (!_bEnded &&
                _b->canGoRight())
            {
                _b->goRight();
                ++_bCount;
            }
            else
                _bEnded = true;
                
        }
        
        virtual void goUp()
        override
        {
            assert(_aCount > 0 || _bCount > 0);
            
            if (_aCount == _bCount)
            {
                _a->goUp();
                _b->goUp();
                --_aCount;
                --_bCount;
                _aEnded = false;
                _bEnded = false;
            }
            else if (_aCount > _bCount)
            {
                _a->goUp();
                --_aCount;
                _aEnded = false;
            }
            else if (_bCount > _aCount)
            {
                _b->goUp();
                --_bCount;
                _bEnded = false;
            }
            
        }
        
        virtual void save()
        override
        {
            _a->save();
            _b->save();
            
            _saveAEnded = _aEnded;
            _saveBEnded = _bEnded;
            _saveACount = _aCount;
            _saveBCount = _bCount;
        }
        
        virtual void restore()
        override
        {
            _a->restore();
            _b->restore();
            
            _aEnded = _saveAEnded;
            _bEnded = _saveBEnded;
            _aCount = _saveACount;
            _bCount = _saveBCount;
        }
        
    
    };

}
#endif