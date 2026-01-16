#ifndef BEE_FISH__DATABASE__AND_PATH_HPP
#define BEE_FISH__DATABASE__AND_PATH_HPP

#include "Iterable.hpp"


namespace BeeFishDatabase {

using namespace std;
using namespace BeeFishPowerEncoding;

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
                Iterable<T>::canGoLeft(branch) and
                _a->canGoLeft(branchA) and
                _b->canGoLeft(branchB);
        }
        
        virtual bool canGoRight(const Branch& branch) const
        override
        {
            Branch branchA = _a->getBranch();
            Branch branchB = _b->getBranch();
            
            return
                Iterable<T>::canGoRight(branch) and
                _a->canGoRight(branchA) and
                _b->canGoRight(branchB);
        }
        
        virtual void goLeft(const Branch& branch)
        override
        {
            _a->goLeft(branch);
            _b->goLeft(branch);
            Iterable<T>::goLeft(branch);
        }
        
        virtual void goRight(const Branch& branch)
        override
        {
            _a->goRight(branch);
            _b->goRight(branch);
            Iterable<T>::goRight(branch);
        }

    
    };

}
#endif