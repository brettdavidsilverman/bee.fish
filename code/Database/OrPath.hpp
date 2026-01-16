#ifndef BEE_FISH__DATABASE__OR_PATH_HPP
#define BEE_FISH__DATABASE__OR_PATH_HPP

#include "Iterable.hpp"


namespace BeeFishDatabase {

using namespace std;
using namespace BeeFishPowerEncoding;

    template<typename T>
    class OrPath :
        public Iterable<T>
    {
    protected:
        Iterable<T>* _a;
        Iterable<T>* _b;
        bool _aEnded;
        bool _bEnded;
        
    public:

        OrPath(Iterable<T>* a,
               Iterable<T>* b) :
            Iterable<T>(*a),
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
   
        
        virtual bool canGoLeft(const Branch& branch) const
        override
        {
            Branch branchA = _a->getBranch();
            Branch branchB = _b->getBranch();
            
            return
                Iterable<T>::canGoLeft(branch) and
                (
                    _a->canGoLeft(branchA) or
                    _b->canGoLeft(branchB)
                );
        }
        
        virtual bool canGoRight(const Branch& branch) const
        override
        {
            Branch branchA = _a->getBranch();
            Branch branchB = _b->getBranch();
            
            return
                Iterable<T>::canGoRight(branch) and
                (
                    _a->canGoRight(branchA) or
                    _b->canGoRight(branchB)
                );
        }
        
        virtual void goLeft(const Branch& branch)
        override
        {
            Branch branchA = _a->getBranch();
            Branch branchB = _b->getBranch();
            
            
            if (!_aEnded &&
                 _a->canGoLeft(branchA))
            {
                _a->goLeft(branchA);
            }
            else
                _aEnded = true;
                
            if (!_bEnded &&
                _b->canGoLeft(branchB))
            {
                _b->goLeft(branchB);
            }
            else
                _bEnded = true;
                
            if (!_aEnded || !_bEnded)
                Iterable<T>::goLeft(branch);
        }
        
        virtual void goRight(const Branch& branch)
        override
        {
            if (!_aEnded &&
                 _a->canGoRight(branch))
            {
                _a->goRight(branch);
            }
            else
                _aEnded = true;
                
            if (!_bEnded &&
                _b->canGoRight(branch))
            {
                _b->goRight(branch);
            }
            else
                _bEnded = true;
                
            if (!_aEnded || !_bEnded)
                Iterable<T>::goRight(branch);
        }

    
    };

}
#endif