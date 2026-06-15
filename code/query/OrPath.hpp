#ifndef BEE_FISH__QUERY__OR_PATH_HPP
#define BEE_FISH__QUERY__OR_PATH_HPP

#include "../Database/Database.hpp"

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

        Index _aDepth = 0;
        Index _bDepth = 0;
    
    public:

        OrPath(
            const PathBase& a,
            const PathBase& b
        ) :
            _a(a.copy()),
            _b(b.copy())
        {
        }
        
        OrPath(const OrPath& source) :
            _a(source._a->copy()),
            _b(source._b->copy()),
            _aDepth(source._aDepth),
            _bDepth(source._bDepth)
        {
        }
        
        virtual ~OrPath() {
            delete _a;
            delete _b;
        }
        
        virtual PathBase* copy() const
        override
        {
            return new OrPath(*this);
                
        }
   
        
        virtual bool canGoLeft() const
        override
        {
            return
            (
                (_aDepth >= _bDepth and 
                 _a->canGoLeft())
                or
                (_bDepth >= _aDepth and 
                _b->canGoLeft())
            );
        }
        
        virtual bool canGoRight() const
        override
        {
            return
            (
                (_aDepth >= _bDepth and
                 _a->canGoRight())
                or
                (_bDepth >= _aDepth and
                 _b->canGoRight())
            );
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
            
            
        }
        

    };

}
#endif