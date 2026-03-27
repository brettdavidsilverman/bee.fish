#ifndef BEE_FISH__QUERY__NOT_PATH_HPP
#define BEE_FISH__QUERY__NOT_PATH_HPP

#include "../Database/Database.hpp"
#include "JoinPathBase.hpp"

namespace BeeFishQuery {

using namespace std;
using namespace BeeFishPowerEncoding;
using namespace BeeFishDatabase;

    class NotPath :
        public JoinPathBase<JSONPath::Id>
    {
    protected:
        PathBase* _path;
        PathBase* _bounds;
        Stack _stack;
        Index _depth = 0;
        Stack _saveStack;
        Index _saveDepth;
        
    public:

        NotPath(PathBase* path, PathBase* bounds) :
            _path(path),
            _bounds(bounds)
        {
        
            if (_path->isDeadEnd())
                _depth = 1;
            else
                _depth = 0;
            
        }
   
        virtual ~NotPath()
        {
            delete _path;
            delete _bounds;
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

        virtual void save()
        {
            _saveStack = _stack;
            _saveDepth = _depth;
            _path->save();
            _bounds->save();
        }
        
        virtual void restore()
        {
            _stack = _saveStack;
            _depth = _saveDepth;
            _path->restore();
            _bounds->restore();
        }

    
    };

}
#endif