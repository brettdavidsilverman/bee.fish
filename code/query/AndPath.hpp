#ifndef BEE_FISH__QUERY__AND_PATH_HPP
#define BEE_FISH__QUERY__AND_PATH_HPP

#include "../Database/Database.hpp"

namespace BeeFishQuery {

using namespace std;
using namespace BeeFishPowerEncoding;
using namespace BeeFishDatabase;

    template<typename T>
    class AndPath :
        public JoinPathBase<T>
    {
    protected:
        JoinPathBase<T>* _a;
        JoinPathBase<T>* _b;
        
    public:

        AndPath( JoinPathBase<T>* a,
                 JoinPathBase<T>* b) :
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
        
        virtual void reset() override
        {
            _a->reset();
            _b->reset();
        }
        
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
        

    
    };

}
#endif