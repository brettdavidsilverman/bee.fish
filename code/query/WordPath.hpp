#ifndef BEE_FISH__QUERY__WORD_PATH_HPP
#define BEE_FISH__QUERY__WORD_PATH_HPP

#include "../Database/Database.hpp"
#include "JoinPathBase.hpp"

namespace BeeFishQuery {

using namespace std;
using namespace BeeFishPowerEncoding;
using namespace BeeFishDatabase;

    class WordPath :
        public JoinPathBase<JSONPath::Id>
    {
    protected:
        PathBase* _a;
        PathBase* _bounds;
        
    public:

        WordPath( PathBase* a,
                 PathBase* bounds) :
            _a(a),
            _bounds(bounds)
        {
        }
   
        virtual ~WordPath()
        {
            delete _a;
            delete _bounds;
        }
        
        
        virtual bool canGoLeft() const
        override
        {
            return
                _a->canGoLeft() and
                _bounds->canGoLeft();
        }
        
        virtual bool canGoRight() const
        override
        {
            return
                _a->canGoRight() and
                _bounds->canGoRight();
        }
        
        virtual void goLeft()
        override
        {
            _a->goLeft();
            _bounds->goLeft();
    
        }
        
        virtual void goRight()
        override
        {
            _a->goRight();
            _bounds->goRight();
        }
        
        virtual void goUp()
        override
        {
            _a->goUp();
            _bounds->goUp();
        }

        virtual void save()
        override
        {
            _a->save();
            _bounds->save();
        }
        
        virtual void restore()
        override
        {
            _a->restore();
            _bounds->restore();
        }
    
    };

}
#endif