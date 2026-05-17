#ifndef BEE_FISH__QUERY__NOT_PATH_HPP
#define BEE_FISH__QUERY__NOT_PATH_HPP

#include "../Database/Database.hpp"

namespace BeeFishQuery {

using namespace std;
using namespace BeeFishPowerEncoding;
using namespace BeeFishDatabase;

    class NotPath :
        public PathBase
    {
    protected:
        PathBase* _path;
        PathBase* _bounds;
        Stack _stack;
        Index _depth = 0;
        
    public:

        NotPath(
            PathBase* path,
            PathBase* bounds
        ) :
            _path(path),
            _bounds(bounds)
        {
        
            
        }
        
        NotPath(const NotPath& source) :
            _path(source._path->copy()),
            _bounds(source._bounds->copy()),
            _stack(source._stack),
            _depth(source._depth)
        {
        }
   
        virtual ~NotPath()
        {
            delete _path;
            delete _bounds;
        }
        
        
        
        virtual PathBase* copy() const
        override
        {
            return new NotPath(*this);
        }
        
        virtual bool canGoLeft() const
        override
        {
            if (_depth == 0 and
                _stack.count() == 1)
            {
                return
                    !_path->canGoLeft() and
                    _bounds->canGoLeft();
            }
            return _bounds->canGoLeft();
            
        }
        
        virtual bool canGoRight() const
        override
        {
            return _bounds->canGoRight();
        }
        
        virtual void goLeft()
        override
        {
            if (_depth == 0 &&
                _path->canGoLeft())
            {
                 _path->goLeft();
            }
            else
                ++_depth;
            _stack.push_back(0);
            _bounds->goLeft();
        }
        
        virtual void goRight()
        override
        {
            if (_depth == 0 &&
                _path->canGoRight())
            {
                 _path->goRight();
            }
            else
                ++_depth;
            _stack.push_back(1);
            _bounds->goRight();
        }
        
        virtual void goUp()
        override
        {
            if (_depth == 0)
                _path->goUp();
            else {
                --_depth;
            }
            _bounds->goUp();
            _stack.pop_back();
            
        }

    };

}
#endif