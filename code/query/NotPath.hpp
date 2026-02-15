#ifndef BEE_FISH__QUERY__NOT_PATH_HPP
#define BEE_FISH__QUERY__NOT_PATH_HPP

#include "../Database/Database.hpp"
#include "JoinPathBase.hpp"

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
        Index _count;
        Stack _saveStack;
        Index _saveCount;
        
    public:

        NotPath(PathBase* path, PathBase* bounds) :
            _path(path),
            _bounds(bounds)
        {
            if (_path->isDeadEnd())
                _count = 1;
            else
                _count = 0;
        }
   
        virtual ~NotPath()
        {
            delete _path;
            delete _bounds;
        }
        
        virtual bool canGoLeft() const
        override
        {
            if (_count == 0)
            {
                if (_stack.count() == 1)
                    return false;
            }
                    
            if (_stack.size())
            {
                if (_stack.count() == 0)
                    return false;
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
            if (_count == 0 &&
                _path->canGoLeft())
            {
                 _path->goLeft();
            }
            else
                ++_count;
            _stack.push_back(0);
            _bounds->goLeft();
        }
        
        virtual void goRight()
        override
        {
            if (_count == 0 &&
                _path->canGoRight())
            {
                 _path->goRight();
            }
            else
                ++_count;
            _stack.push_back(1);
            _bounds->goRight();
        }
        
        virtual void goUp()
        override
        {
            if (_count == 0)
                _path->goUp();
            else {
                --_count;
            }
            _bounds->goUp();
            _stack.pop_back();
            
        }
        /*
        virtual bool isDeadEnd() const
        override
        {
            if (_stack.size())
                return _stack.count() == 0;
            return false;
        }
        */
        virtual void save()
        {
            _saveStack = _stack;
            _saveCount = _count;
            _path->save();
            _bounds->save();
        }
        
        virtual void restore()
        {
            _stack = _saveStack;
            _count = _saveCount;
            _path->restore();
            _bounds->restore();
        }

    
    };

}
#endif