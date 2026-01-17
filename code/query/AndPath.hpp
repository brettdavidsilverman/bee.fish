#ifndef BEE_FISH__QUERY__AND_PATH_HPP
#define BEE_FISH__QUERY__AND_PATH_HPP

#include "../Database/Database.hpp"

namespace BeeFishQuery {

using namespace std;
using namespace BeeFishPowerEncoding;
using namespace BeeFishDatabase;

    template<typename T>
    class AndPath :
        public PathBase
    {
    protected:
        Iterable<T>* _a;
        Iterable<T>* _b;
        
    public:

        AndPath( Iterable<T>* a,
                 Iterable<T>* b) :
            _a(a),
            _b(b)
        {
        }
   
        virtual ~AndPath()
        {
            delete _a;
            delete _b;
        }
        
        virtual bool canGoLeft() const
        override
        {
            Branch branchA = _a->getBranch();
            Branch branchB = _b->getBranch();
            
            return
                _a->canGoLeft(branchA) and
                _b->canGoLeft(branchB);
        }
        
        virtual bool canGoRight() const
        override
        {
            Branch branchA = _a->getBranch();
            Branch branchB = _b->getBranch();
            
            return
                _a->canGoRight(branchA) and
                _b->canGoRight(branchB);
        }
        
        virtual void goLeft()
        override
        {
            Branch branchA = _a->getBranch();
            Branch branchB = _b->getBranch();
            
            _a->goLeft(branchA);
            _b->goLeft(branchB);
    
        }
        
        virtual void goRight()
        override
        {
            Branch branchA = _a->getBranch();
            Branch branchB = _b->getBranch();
            
            _a->goRight(branchA);
            _b->goRight(branchB);
        }
        
        virtual void goUp()
        override
        {
            _a->goUp();
            _b->goUp();
        }
        
        virtual bool isDeadEnd() const
        override
        {
            Branch branchA = _a->getBranch();
            Branch branchB = _b->getBranch();
            
            bool isDeadEnd =
            not (
                    (
                        _a->canGoLeft(branchA) and
                        _b->canGoLeft(branchB)
                    ) or
                    (
                        _a->canGoRight(branchA) and
                        _b->canGoRight(branchB)
                    )
                );
                
            return isDeadEnd;
                
        }

    
    };

}
#endif