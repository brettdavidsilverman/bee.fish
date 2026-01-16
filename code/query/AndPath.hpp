#ifndef BEE_FISH__QUERY__AND_PATH_HPP
#define BEE_FISH__QUERY__AND_PATH_HPP

#include "../Database/Iterable.hpp"


namespace BeeFishQuery {

using namespace std;
using namespace BeeFishPowerEncoding;
using namespace BeeFishDatabase;

    template<typename T>
    class AndPath :
        public Iterable<T>
    {
    protected:
        Iterable<T>* _a;
        Iterable<T>* _b;
        
    public:

        AndPath( Iterable<T>* a,
                 Iterable<T>* b) :
            Iterable<T>(*a),
            _a(a),
            _b(b)
        {
        }
   
        virtual ~AndPath()
        {
            delete _a;
            delete _b;
        }
        
        virtual bool canGoLeft(const Branch& branch) const
        override
        {
            Branch branchA = _a->getBranch();
            Branch branchB = _b->getBranch();
            
            return
                _a->canGoLeft(branchA) and
                _b->canGoLeft(branchB);
        }
        
        virtual bool canGoRight(const Branch& branch) const
        override
        {
            Branch branchA = _a->getBranch();
            Branch branchB = _b->getBranch();
            
            return
                _a->canGoRight(branchA) and
                _b->canGoRight(branchB);
        }
        
        virtual void goLeft(const Branch& branch)
        override
        {
            Branch branchA = _a->getBranch();
            Branch branchB = _b->getBranch();
            
            _a->goLeft(branchA);
            _b->goLeft(branchB);
    
        }
        
        virtual void goRight(const Branch& branch)
        override
        {
            Branch branchA = _a->getBranch();
            Branch branchB = _b->getBranch();
            
            _a->goRight(branchA);
            _b->goRight(branchB);
        }
        
        virtual bool isDeadEnd(const Branch& branch) const
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
                
            assert(isDeadEnd);
            
            return isDeadEnd;
                
        }

    
    };

}
#endif