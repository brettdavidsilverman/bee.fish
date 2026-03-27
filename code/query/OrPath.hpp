#ifndef BEE_FISH__QUERY__OR_PATH_HPP
#define BEE_FISH__QUERY__OR_PATH_HPP

#include "../Database/Database.hpp"

#include "JoinPathBase.hpp"

namespace BeeFishQuery {

using namespace std;
using namespace BeeFishPowerEncoding;
using namespace BeeFishDatabase;

    class OrPath :
        public JoinPathBase<JSONPath::Id>
    {
    protected:
        PathBase* _a;
        PathBase* _b;

        Index _aDepth = 0;
        Index _bDepth = 0;
        
        Index _saveADepth = 0;
        Index _saveBDepth = 0;
        
    public:

        OrPath(PathBase* a,
               PathBase* b) :
            _a(a),
            _b(b)
        {
        }
        
        virtual ~OrPath() {
            delete _a;
            delete _b;
        }
   
        
        virtual bool canGoLeft() const
        override
        {
            return
                (_aDepth >= _bDepth and 
                 _a->canGoLeft())
                or
                (_bDepth >= _aDepth and 
                _b->canGoLeft());
        }
        
        virtual bool canGoRight() const
        override
        {
            return
                (_aDepth >= _bDepth and
                 _a->canGoRight())
                or
                (_bDepth >= _aDepth and
                 _b->canGoRight());
        }
        
        virtual void goLeft()
        override
        {
            bool incrementA = false;
            bool incrementB = false;
            
            if (_aDepth >= _bDepth and
                _a->canGoLeft())
            {
                _a->goLeft();
                incrementA = true;
            }
        
            
            if (_bDepth >= _aDepth and
                 _b->canGoLeft())
            {
                _b->goLeft();
                incrementB = true;
            }
        
                
            if (incrementA)
                ++_aDepth;
                
            if (incrementB)
                ++_bDepth;
                
        }
        
        
        
        virtual void goRight()
        override
        {
            
            bool incrementA = false;
            bool incrementB = false;
            
            if (_aDepth >= _bDepth and 
                _a->canGoRight())
            {
                _a->goRight();
                incrementA = true;
            }
    
            if (_bDepth >= _aDepth and 
                _b->canGoRight())
            {
                _b->goRight();
                incrementB = true;
            }
            
            if (incrementA)
                ++_aDepth;
                
            if (incrementB)
                ++_bDepth;
        }

        virtual void goUp()
        override
        {
            assert(_aDepth > 0 || _bDepth > 0);
            
            if (_aDepth == _bDepth)
            {
                _a->goUp();
                _b->goUp();
                --_aDepth;
                --_bDepth;
            }
            else if (_aDepth > _bDepth)
            {
                _a->goUp();
                --_aDepth;
            }
            else if (_bDepth > _aDepth)
            {
                _b->goUp();
                --_bDepth;
            }
            else
                assert(false);
            
        }
        
        virtual void save()
        override
        {
            _a->save();
            _b->save();
            
            _saveADepth = _aDepth;
            _saveBDepth = _bDepth;
            
        }
        
        virtual void restore()
        override
        {
            _a->restore();
            _b->restore();
            
            _aDepth = _saveADepth;
            _bDepth = _saveBDepth;
            
        }
        
    
    };

}
#endif